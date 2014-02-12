// Copyright 2014. All Rights Reserved.
// Author: yeshunping@gmail.com (Shunping Ye)

#ifndef PROTORPC_HTTP_SERVER_DEBUG_UTIL_H_
#define PROTORPC_HTTP_SERVER_DEBUG_UTIL_H_

struct evhttp_request;

namespace protorpc {

void dump_request_cb(struct evhttp_request *req);

}

#endif  // PROTORPC_HTTP_SERVER_DEBUG_UTIL_H_
