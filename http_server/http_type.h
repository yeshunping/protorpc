// Copyright 2014. All Rights Reserved.
// Author: yeshunping@gmail.com (Shunping Ye)

#ifndef PROTORPC_HTTP_SERVER_HTTP_TYPE_H_
#define PROTORPC_HTTP_SERVER_HTTP_TYPE_H_

#include <string>

namespace protorpc {

//  Try to guess a good content-type for 'path', return "application/misc" as default type
std::string GuessContentType(const std::string& path);

}
#endif  // PROTORPC_HTTP_SERVER_HTTP_TYPE_H_
