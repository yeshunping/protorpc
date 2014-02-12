// Copyright 2014. All Rights Reserved.
// Author: yeshunping@gmail.com (Shunping Ye)

#include "protorpc/http_server/debug_util.h"

#include <stdio.h>

#include "thirdparty/event/include/event2/buffer.h"
#include "thirdparty/event/include/event2/http.h"
#include "thirdparty/event/include/event2/keyvalq_struct.h"

namespace protorpc {
void dump_request_cb(struct evhttp_request *req) {
  const char *cmdtype;
  struct evkeyvalq *headers;
  struct evkeyval *header;
  struct evbuffer *buf;

  switch (evhttp_request_get_command(req)) {
    case EVHTTP_REQ_GET:
      cmdtype = "GET";
      break;
    case EVHTTP_REQ_POST:
      cmdtype = "POST";
      break;
    case EVHTTP_REQ_HEAD:
      cmdtype = "HEAD";
      break;
    case EVHTTP_REQ_PUT:
      cmdtype = "PUT";
      break;
    case EVHTTP_REQ_DELETE:
      cmdtype = "DELETE";
      break;
    case EVHTTP_REQ_OPTIONS:
      cmdtype = "OPTIONS";
      break;
    case EVHTTP_REQ_TRACE:
      cmdtype = "TRACE";
      break;
    case EVHTTP_REQ_CONNECT:
      cmdtype = "CONNECT";
      break;
    case EVHTTP_REQ_PATCH:
      cmdtype = "PATCH";
      break;
    default:
      cmdtype = "unknown";
      break;
  }

  printf("Received a %s request for %s\nHeaders:\n", cmdtype,
         evhttp_request_get_uri(req));

  headers = evhttp_request_get_input_headers(req);
  for (header = headers->tqh_first; header; header = header->next.tqe_next) {
    printf("  %s: %s\n", header->key, header->value);
  }

  buf = evhttp_request_get_input_buffer(req);
  puts("Input data: <<<");
  while (evbuffer_get_length(buf)) {
    int n;
    char cbuf[128];
    n = evbuffer_remove(buf, cbuf, sizeof(buf) - 1);
    if (n > 0)
      (void) fwrite(cbuf, 1, n, stdout);
  }
  puts(">>>");
}
}  // namespace protorpc
