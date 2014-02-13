// Copyright 2014. All Rights Reserved.
// Author: yeshunping@gmail.com (Shunping Ye)

#ifndef PROTORPC_HTTP_SERVER_HTTP_HANDLER_H_
#define PROTORPC_HTTP_SERVER_HTTP_HANDLER_H_

#include "base/callback.h"

namespace protorpc {
class HttpRequest;
class HttpResponse;

class HttpHandler {
 public:
  HttpHandler(base::ResultCallback2<bool, HttpRequest*, HttpResponse*>* callback);
  virtual ~HttpHandler();

  virtual bool Handler(HttpRequest* request, HttpResponse* response) = 0;

 protected:
  base::ResultCallback2<bool, HttpRequest*, HttpResponse*>* callback_;

 private:
  DISALLOW_COPY_AND_ASSIGN(HttpHandler);
};

class DefaultHttpHandler : public HttpHandler {
 public:
  DefaultHttpHandler(base::ResultCallback2<bool, HttpRequest*, HttpResponse*>* callback);
  virtual ~DefaultHttpHandler();

  //  Call callback function, then send back response.
  virtual bool Handler(HttpRequest* request, HttpResponse* response);
 private:

  DISALLOW_COPY_AND_ASSIGN(DefaultHttpHandler);
};
}  // namespace protorpc
#endif  // PROTORPC_HTTP_SERVER_HTTP_HANDLER_H_
