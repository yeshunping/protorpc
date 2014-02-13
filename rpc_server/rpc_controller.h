// Copyright 2014. All Rights Reserved.
// Author: yeshunping@gmail.com (Shunping Ye)

#ifndef PROTORPC_RPC_SERVER_RPC_CONTROLLER_H
#define PROTORPC_RPC_SERVER_RPC_CONTROLLER_H

#include <string>

#include "base/basictypes.h"
#include "thirdparty/protobuf/include/google/protobuf/service.h"

namespace protorpc {

class RpcController : public google::protobuf::RpcController {
 public:
  RpcController();
  ~RpcController();

  virtual void Reset();
  virtual bool Failed() const;
  virtual std::string ErrorText() const;
  virtual void StartCancel();
  virtual void SetFailed(const std::string& reason);
  virtual bool IsCanceled() const;
  virtual void NotifyOnCancel(google::protobuf::Closure* callback);
 private:
  bool failed_;
  bool canceled_;
  std::string error_reason_;
  DISALLOW_COPY_AND_ASSIGN(RpcController);
};

}  // namespace poppy

#endif // PROTORPC_RPC_SERVER_RPC_CONTROLLER_H
