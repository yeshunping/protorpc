// Copyright 2014. All Rights Reserved.
// Author: yeshunping@gmail.com (Shunping Ye)

#include "protorpc/http_server/http_request.h"

#include "base/basictypes.h"
#include "thirdparty/event/include/event2/http.h"
#include "thirdparty/event/include/event2/http_struct.h"

namespace protorpc {

HttpRequest::HttpRequest(struct evhttp_request* request) : request_(request) {
}

HttpRequest::~HttpRequest() {
}

const std::string HttpRequest::Url() {
  return request_->uri;
}

}  // namespace protorpc
