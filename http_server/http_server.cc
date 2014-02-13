// Copyright 2014. All Rights Reserved.
// Author: yeshunping@gmail.com (Shunping Ye)

#include "protorpc/http_server/http_server.h"

#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>

#include "base/logging.h"
#include "base/flags.h"
#include "base/string_util.h"
#include "base/stl_util-inl.h"
#include "protorpc/http_server/event_loop.h"
#include "protorpc/http_server/debug_util.h"
#include "protorpc/http_server/http_type.h"
#include "protorpc/http_server/http_request.h"
#include "protorpc/http_server/http_response.h"
#include "protorpc/http_server/http_handler.h"
#include "thirdparty/event/include/event2/http.h"
#include "thirdparty/event/include/event2/buffer.h"

DEFINE_string(doc_root, "", "");
DEFINE_bool(use_default_handler, true, "");

namespace protorpc {

/* This callback gets invoked when we get any http request that doesn't match
 * any other callback.  Like any evhttp server callback, it has a simple job:
 * it must eventually call evhttp_send_error() or evhttp_send_reply().
 */
void send_document_cb(struct evhttp_request *req, void *arg) {
  HttpServer* server = (HttpServer*)arg;
  struct evbuffer *evb = NULL;
  const char *uri = evhttp_request_get_uri(req);
  struct evhttp_uri *decoded = NULL;
  const char *path;
  char *decoded_path;
  char *whole_path = NULL;
  size_t len;
  int fd = -1;
  struct stat st;

  if (evhttp_request_get_command(req) != EVHTTP_REQ_GET) {
    evhttp_send_reply(req, 200, "OK", NULL);
    return;
  }

  printf("Got a GET request for <%s>\n", uri);

  /* Decode the URI */
  decoded = evhttp_uri_parse(uri);
  if (!decoded) {
    printf("It's not a good URI. Sending BADREQUEST\n");
    evhttp_send_error(req, HTTP_BADREQUEST, 0);
    return;
  }

  /* Let's see what path the user asked for. */
  path = evhttp_uri_get_path(decoded);
  if (!path)
    path = "/";

  /* We need to decode it, to see what path the user really wanted. */
  decoded_path = evhttp_uridecode(path, 0, NULL);
  if (decoded_path == NULL)
    goto err;
  /* Don't allow any ".."s in the path, to avoid exposing stuff outside
   * of the docroot.  This test is both overzealous and underzealous:
   * it forbids aceptable paths like "/this/one..here", but it doesn't
   * do anything to prevent symlink following." */
  if (strstr(decoded_path, ".."))
    goto err;

  len = strlen(decoded_path) + FLAGS_doc_root.length() + 2;
  if (!(whole_path = (char*)malloc(len))) {
    perror("malloc");
    goto err;
  }
  evutil_snprintf(whole_path, len, "%s/%s", FLAGS_doc_root.c_str(), decoded_path);

  if (stat(whole_path, &st) < 0) {
    goto err;
  }

  /* This holds the content we're sending. */
  evb = evbuffer_new();

  if (S_ISDIR(st.st_mode)) {
    /* If it's a directory, read the comments and make a little
     * index page */
    DIR *d;
    struct dirent *ent;
    const char *trailing_slash = "";

    if (!strlen(path) || path[strlen(path) - 1] != '/')
      trailing_slash = "/";


    if (!(d = opendir(whole_path)))
      goto err;

    evbuffer_add_printf(evb, "<html>\n <head>\n"
                        "  <title>%s</title>\n"
                        "  <base href='%s%s%s'>\n"
                        " </head>\n"
                        " <body>\n"
                        "  <h1>%s</h1>\n"
                        "  <ul>\n",
                        decoded_path, /* XXX html-escape this. */
                        server->uri_root().c_str(), path, /* XXX html-escape this? */
                        trailing_slash,
                        decoded_path /* XXX html-escape this */);
    while ((ent = readdir(d))) {
      const char *name = ent->d_name;
      evbuffer_add_printf(evb, "    <li><a href=\"%s\">%s</a>\n", name, name);/* XXX escape this */
    }
    evbuffer_add_printf(evb, "</ul></body></html>\n");
    closedir(d);
    evhttp_add_header(evhttp_request_get_output_headers(req), "Content-Type",
                      "text/html");
  } else {
    /* Otherwise it's a file; add it to the buffer to get
     * sent via sendfile */
    string type = GuessContentType(decoded_path);
    if ((fd = open(whole_path, O_RDONLY)) < 0) {
      perror("open");
      goto err;
    }

    if (fstat(fd, &st) < 0) {
      /* Make sure the length still matches, now that we
       * opened the file :/ */
      perror("fstat");
      goto err;
    }
    evhttp_add_header(evhttp_request_get_output_headers(req), "Content-Type",
                      type.c_str());
    evbuffer_add_file(evb, fd, 0, st.st_size);
  }

  evhttp_send_reply(req, 200, "OK", evb);
  goto done;
  err: evhttp_send_error(req, 404, "Document was not found");
  if (fd >= 0)
    close(fd);
  done: if (decoded)
    evhttp_uri_free(decoded);
  if (decoded_path)
    free(decoded_path);
  if (whole_path)
    free(whole_path);
  if (evb)
    evbuffer_free(evb);
}

HttpServer::HttpServer(EventLoop* event_loop, int listen_port)
  : listen_port_(listen_port) {
  /* Create a new evhttp object to handle requests. */
  http_ = evhttp_new(event_loop->base());
  CHECK(http_) << "couldn't create evhttp. Exiting.";

  if (FLAGS_use_default_handler) {
    //  Set Default handler, send static documents for request path.
    evhttp_set_gencb(http_, send_document_cb, this);
  }
}

HttpServer::~HttpServer() {
  base::STLDeleteValues(&handlers_);
}

static void InnerHandlerCallback(struct evhttp_request * request,
                                 void* cb) {
  HttpHandler* handler = (HttpHandler*)cb;
  HttpRequest http_request(request);
  HttpResponse response(&http_request);
  handler->Handler(&http_request, &response);
}

bool HttpServer::RegisterHttpHandler(const string& path,
                                     HttpHandler* handler) {
  hash_map<std::string, HttpHandler*>::const_iterator it = handlers_.find(path);
  if (it != handlers_.end()) {
    LOG(ERROR) << "Handler for path :" << path << " exist, fail to register new handler for it";
    return false;
  }
  evhttp_set_cb(http_, path.c_str(), InnerHandlerCallback, (void*)handler);
  handlers_.insert(make_pair(path, handler));
  return true;
}

void HttpServer::Start() {
  /* Now we tell the evhttp what port to listen on */
  struct evhttp_bound_socket *handle = evhttp_bind_socket_with_handle(http_, "0.0.0.0", (uint16_t)listen_port_);
  CHECK(handle)  << "couldn't bind to port "  << listen_port_ << ", Exiting.";
  ShowBindInfo(handle);
}


void HttpServer::ShowBindInfo(struct evhttp_bound_socket *handle) {
  /* Extract and display the address we're listening on. */
  struct sockaddr_storage ss;
  evutil_socket_t fd;
  ev_socklen_t socklen = sizeof(ss);
  char addrbuf[128];
  void *inaddr;
  const char *addr;
  int got_port = -1;
  fd = evhttp_bound_socket_get_fd(handle);
  memset(&ss, 0, sizeof(ss));
  if (getsockname(fd, (struct sockaddr *) &ss, &socklen)) {
    perror("getsockname() failed");
    return;
  }

  if (ss.ss_family == AF_INET) {
    got_port = ntohs(((struct sockaddr_in*) &ss)->sin_port);
    inaddr = &((struct sockaddr_in*) &ss)->sin_addr;
  } else if (ss.ss_family == AF_INET6) {
    got_port = ntohs(((struct sockaddr_in6*) &ss)->sin6_port);
    inaddr = &((struct sockaddr_in6*) &ss)->sin6_addr;
  } else {
    fprintf(stderr, "Weird address family %d\n", ss.ss_family);
    return;
  }

  addr = evutil_inet_ntop(ss.ss_family, inaddr, addrbuf, sizeof(addrbuf));
  if (addr) {
    LOG(INFO) << "Listening on " << addr << ":" << got_port;
    uri_root_ = StringPrintf("http://%s:%d", addr, got_port);
  } else {
    LOG(ERROR) << "evutil_inet_ntop failed";
    return;
  }
}
}  // namespace protorpc
