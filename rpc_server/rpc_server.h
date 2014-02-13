// Copyright 2014. All Rights Reserved.
// Author: yeshunping@gmail.com (Shunping Ye)

#ifndef PROTORPC_RPC_SERVER_RPC_SERVER_H
#define PROTORPC_RPC_SERVER_RPC_SERVER_H

#include <string>

#include "protorpc/http_server/http_server.h"
#include "thirdparty/protobuf/include/google/protobuf/service.h"

namespace protorpc {

class RpcServer : public HttpServer {
 public:
  RpcServer(const std::string& binary_path, EventLoop* event_loop,
            int listen_port)
      : HttpServer(event_loop, listen_port),
        binary_path_(binary_path) {
    RegisterBuiltinHandlers();
  }

  virtual ~RpcServer() {
    UnregisterServices();
  }

  void RegisterService(google::protobuf::Service* service);

 private:
  void RegisterBuiltinHandlers();
  void UnregisterServices();

  bool HandleVersionRequest(HttpRequest*, HttpResponse*);
  bool HandleFlagsRequest(HttpRequest*, HttpResponse*);

  std::string binary_path_;
};

}  // namespace poppy

#endif // PROTORPC_RPC_SERVER_RPC_SERVER_H
