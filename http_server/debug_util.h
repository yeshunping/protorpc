// Copyright 2014. All Rights Reserved.
// Author: yeshunping@gmail.com (Shunping Ye)

#ifndef PROTORPC_HTTP_SERVER_DEBUG_UTIL_H_
#define PROTORPC_HTTP_SERVER_DEBUG_UTIL_H_

namespace protorpc {
class HttpRequest;

void DumpRequestInfo(HttpRequest* request);

}

#endif  // PROTORPC_HTTP_SERVER_DEBUG_UTIL_H_
