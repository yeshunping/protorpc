// Copyright 2014. All Rights Reserved.
// Author: yeshunping@gmail.com (Shunping Ye)

#ifndef PROTORPC_RPC_CLIENT_RPC_CLIENT_H
#define PROTORPC_RPC_CLIENT_RPC_CLIENT_H

#include "protorpc/http_client/http_client.h"

namespace protorpc {
class RpcClient : public CurlWrapper {
 public :
  RpcClient();
   ~RpcClient();

 private:
   DISALLOW_COPY_AND_ASSIGN(RpcClient);
};
}
#endif  // PROTORPC_RPC_CLIENT_RPC_CLIENT_H
