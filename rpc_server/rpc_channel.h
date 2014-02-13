// Copyright 2014. All Rights Reserved.
// Author: yeshunping@gmail.com (Shunping Ye)

#ifndef PROTORPC_RPC_SERVER_RPC_SERVER_H
#define PROTORPC_RPC_SERVER_RPC_SERVER_H

#include <string>

#include "base/basictypes.h"
#include "thirdparty/protobuf/include/google/protobuf/service.h"

namespace protorpc {
class RpcClient;

class RpcChannel : public google::protobuf::RpcChannel {
 public:
  RpcChannel(protorpc::RpcClient* client, const std::string& server_host, int port);
  virtual ~RpcChannel();

  virtual void CallMethod(const google::protobuf::MethodDescriptor* method,
                          google::protobuf::RpcController* controller,
                          const google::protobuf::Message* request,
                          google::protobuf::Message* response,
                          google::protobuf::Closure* done);

 private:
  protorpc::RpcClient* client_;
  std::string host_;
  int port_;
  DISALLOW_COPY_AND_ASSIGN(RpcChannel);
};

}  // namespace poppy

#endif // PROTORPC_RPC_SERVER_RPC_SERVER_H
