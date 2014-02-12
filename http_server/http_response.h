// Copyright 2014. All Rights Reserved.
// Author: yeshunping@gmail.com (Shunping Ye)

#ifndef PROTORPC_HTTP_SERVER_HTTP_RESPONSE_H_
#define PROTORPC_HTTP_SERVER_HTTP_RESPONSE_H_

#include "base/basictypes.h"

namespace protorpc {
class HttpResponse {
 public:
  HttpResponse();
  ~HttpResponse();

 private:
  DISALLOW_COPY_AND_ASSIGN(HttpResponse);
};
}  // namespace protorpc

#endif  // PROTORPC_HTTP_SERVER_HTTP_RESPONSE_H_
