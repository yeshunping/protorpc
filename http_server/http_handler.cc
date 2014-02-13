// Copyright 2014. All Rights Reserved.
// Author: yeshunping@gmail.com (Shunping Ye)

#include "protorpc/http_server/http_handler.h"

#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>

#include "base/logging.h"
#include "base/flags.h"
#include "base/string_util.h"
#include "protorpc/http_server/http_request.h"
#include "protorpc/http_server/http_response.h"

namespace protorpc {
HttpHandler::HttpHandler(base::ResultCallback2<bool, HttpRequest*, HttpResponse*>* callback)
  : callback_(callback) {
}

HttpHandler::~HttpHandler() {
}

DefaultHttpHandler::DefaultHttpHandler(base::ResultCallback2<bool, HttpRequest*, HttpResponse*>* callback)
  : HttpHandler(callback) {
}

DefaultHttpHandler::~DefaultHttpHandler() {
}

bool DefaultHttpHandler::Handler(HttpRequest* request, HttpResponse* response) {
  VLOG(1) << "receive request:" << request->Url();
  if (FLAGS_v >= 3) {
    request->Dump();
  }
  if (!callback_->Run(request, response)) {
    LOG(WARNING) << "fail to treat request:" << request->Url();
  }
  return response->Send();
}
}  // namespace protorpc
