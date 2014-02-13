// Copyright 2014. All Rights Reserved.
// Author: yeshunping@gmail.com (Shunping Ye)

#ifndef PROTORPC_RPC_SERVER_RPC_SERVER_H
#define PROTORPC_RPC_SERVER_RPC_SERVER_H

#include <string>

#include "base/hash_tables.h"
#include "protorpc/http_server/http_server.h"
#include "thirdparty/protobuf/include/google/protobuf/service.h"

namespace protorpc {
class RpcController;

//  NewCallback canonot bind more than two params, so here we wrap these two vars.
struct RpcData {
  RpcData(google::protobuf::Message* _request, google::protobuf::Message* _response)
    : request(_request), response(_response) {
  }
  google::protobuf::Message* request;
  google::protobuf::Message* response;
};

class RpcServer : public HttpServer {
 public:
  RpcServer(EventLoop* event_loop,
            int listen_port)
      : HttpServer(event_loop, listen_port) {
    RegisterBuiltinHandlers();
  }

  virtual ~RpcServer() {
    UnregisterServices();
  }

  bool RegisterService(google::protobuf::Service* service);

 private:
  void RegisterBuiltinHandlers();
  void UnregisterServices();

  bool GetRpcRequest(HttpRequest* request, google::protobuf::Message* msg);
  const google::protobuf::MethodDescriptor* GetRpcMethod(const std::string& method_name,
                                                         google::protobuf::Service* service);

  // TODO(yeshunping) : Support status request, profiler request, Form rpc request, json rpc request.
  bool HandleVersionRequest(HttpRequest*, HttpResponse*);
  bool HandleFlagsRequest(HttpRequest*, HttpResponse*);
  bool HandleRpcRequest(HttpRequest*, HttpResponse*);

  void RequestComplete(RpcController* controller,
                       RpcData* data);

  base::hash_map<std::string, google::protobuf::Service*> services_;
  DISALLOW_COPY_AND_ASSIGN(RpcServer);
};

}  // namespace poppy

#endif // PROTORPC_RPC_SERVER_RPC_SERVER_H
