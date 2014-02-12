// Copyright 2014. All Rights Reserved.
// Author: yeshunping@gmail.com (Shunping Ye)

#ifndef PROTORPC_HTTP_SERVER_HTTP_REQUEST_H_
#define PROTORPC_HTTP_SERVER_HTTP_REQUEST_H_

#include "base/basictypes.h"

struct evhttp_request;

namespace protorpc {
class HttpRequest {
 public:
  HttpRequest(struct evhttp_request* request);
  ~HttpRequest();

  struct evhttp_request* request() {
    return request_;
  }

 private:
  struct evhttp_request* request_;
  DISALLOW_COPY_AND_ASSIGN(HttpRequest);
};



}  // namespace protorpc

#endif  // PROTORPC_HTTP_SERVER_HTTP_REQUEST_H_
