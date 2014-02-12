// Copyright 2014. All Rights Reserved.
// Author: yeshunping@gmail.com (Shunping Ye)

#include "protorpc/http_server/http_response.h"

#include "protorpc/http_server/http_request.h"
#include "thirdparty/event/include/event2/buffer.h"
#include "thirdparty/event/include/event2/http.h"

namespace protorpc {

HttpResponse::HttpResponse(protorpc::HttpRequest* request) : request_(request) {
}

HttpResponse::~HttpResponse() {
}

void HttpResponse::AppendHeader(const std::string& buff) {
    header_.append(buff);
}

void HttpResponse::AppendBuffer(const std::string& buff) {
    content_.append(buff);
}

bool HttpResponse::Send() {
    struct evbuffer *evb = evbuffer_new();
    evbuffer_add_printf(evb, content_.c_str());
    evhttp_send_reply(request_->request(), 200, "OK", evb);
    evbuffer_free(evb);
    return true;
}
}  // namespace protorpc
