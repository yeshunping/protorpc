// Copyright 2014. All Rights Reserved.
// Author: yeshunping@gmail.com (Shunping Ye)

#ifndef PROTORPC_HTTP_SERVER_HTTP_RESPONSE_H_
#define PROTORPC_HTTP_SERVER_HTTP_RESPONSE_H_

#include <string>

#include "base/basictypes.h"

namespace protorpc {
class HttpRequest;

class HttpResponse {
 public:
  explicit HttpResponse(protorpc::HttpRequest* request);
  ~HttpResponse();

  void AppendHeader(const std::string& buff);
  void AppendBuffer(const std::string& buff);

  bool Send();
 private:
  std::string header_;
  std::string content_;
  protorpc::HttpRequest* request_;
  DISALLOW_COPY_AND_ASSIGN(HttpResponse);
};
}  // namespace protorpc

#endif  // PROTORPC_HTTP_SERVER_HTTP_RESPONSE_H_
