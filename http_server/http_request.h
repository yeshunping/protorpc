// Copyright 2014. All Rights Reserved.
// Author: yeshunping@gmail.com (Shunping Ye)

#ifndef PROTORPC_HTTP_SERVER_HTTP_REQUEST_H_
#define PROTORPC_HTTP_SERVER_HTTP_REQUEST_H_

#include <string>

#include "base/basictypes.h"

struct evhttp_request;

namespace protorpc {

enum HttpMethodType {
  HttpMethodType_kGet = 1 << 0,
  HttpMethodType_kPost = 1 << 1,
  HttpMethodType_kHead = 1 << 2,
  HttpMethodType_kPut = 1 << 3,
  HttpMethodType_kDelete = 1 << 4,
  HttpMethodType_kOptions = 1 << 5,
  HttpMethodType_kTrace = 1 << 6,
  HttpMethodType_kConnect = 1 << 7,
  HttpMethodType_kPatch = 1 << 8,
  HttpMethodType_kUnknown = 1 << 9
};

class HttpRequest {
 public:
  HttpRequest(struct evhttp_request* request);
  ~HttpRequest();

  struct evhttp_request* request() {
    return request_;
  }
  std::string GetHeader(const std::string& key) const;
  std::string GetRequestData() const;
  HttpMethodType HttpMethod() const;

  const std::string Url();
  //  Dump debug string for request
  void Dump();
 private:
  struct evhttp_request* request_;
  DISALLOW_COPY_AND_ASSIGN(HttpRequest);
};

}  // namespace protorpc

#endif  // PROTORPC_HTTP_SERVER_HTTP_REQUEST_H_
