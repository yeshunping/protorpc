// Copyright 2014. All Rights Reserved.
// Author: yeshunping@gmail.com (Shunping Ye)

#include "protorpc/http_server/http_request.h"

#include <stdio.h>
#include <string>

#include "base/es.h"
#include "base/basictypes.h"
#include "base/string_util.h"
#include "thirdparty/event/include/event2/http.h"
#include "thirdparty/event/include/event2/http_struct.h"
#include "thirdparty/event/include/event2/buffer.h"
#include "thirdparty/event/include/event2/keyvalq_struct.h"

namespace protorpc {

HttpRequest::HttpRequest(struct evhttp_request* request) : request_(request) {
}

HttpRequest::~HttpRequest() {
}

const string HttpRequest::Url() {
  return request_->uri;
}

string HttpRequest::GetHeader(const string& key) const {
  struct evkeyvalq *headers;
  struct evkeyval *header;
  headers = evhttp_request_get_input_headers(request_);
  for (header = headers->tqh_first; header; header = header->next.tqe_next) {
    if (StrCaseCmp(header->key, key.c_str()) == 0) {
      return string(header->value);
    }
  }
  return string();
}

string HttpRequest::GetRequestData() const {
  struct evbuffer *buf = evhttp_request_get_input_buffer(request_);
  string data;
  while (evbuffer_get_length(buf)) {
    int n;
    char cbuf[128];
    n = evbuffer_remove(buf, cbuf, sizeof(buf) - 1);
    if (n > 0) {
      data.append(cbuf, n);
    }
  }
  return data;
}

HttpMethodType HttpRequest::HttpMethod() const {
  switch (evhttp_request_get_command(request_)) {
    case EVHTTP_REQ_GET:
      return HttpMethodType_kGet;
    case EVHTTP_REQ_POST:
      return HttpMethodType_kPost;
    case EVHTTP_REQ_HEAD:
      return HttpMethodType_kHead;
    case EVHTTP_REQ_PUT:
      return HttpMethodType_kPut;
    case EVHTTP_REQ_DELETE:
      return HttpMethodType_kDelete;
    case EVHTTP_REQ_OPTIONS:
      return HttpMethodType_kOptions;
    case EVHTTP_REQ_TRACE:
      return HttpMethodType_kTrace;
    case EVHTTP_REQ_CONNECT:
      return HttpMethodType_kConnect;
    case EVHTTP_REQ_PATCH:
      return HttpMethodType_kPatch;
    default:
      return HttpMethodType_kUnknown;
  }
}

void HttpRequest::Dump() {
  //  Dump request method and Uri
  const char *cmdtype;
  switch (evhttp_request_get_command(request_)) {
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
         evhttp_request_get_uri(request_));

  //  Dump headers
  struct evkeyvalq *headers;
  struct evkeyval *header;
  headers = evhttp_request_get_input_headers(request_);
  for (header = headers->tqh_first; header; header = header->next.tqe_next) {
    printf("  %s: %s\n", header->key, header->value);
  }

  //  Dump input data
  struct evbuffer *buf;
  buf = evhttp_request_get_input_buffer(request_);
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
