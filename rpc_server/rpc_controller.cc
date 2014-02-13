// Copyright 2014. All Rights Reserved.
// Author: yeshunping@gmail.com (Shunping Ye)

#include "protorpc/rpc_server/rpc_controller.h"

#include "base/flags.h"
#include "base/logging.h"

namespace protorpc {
RpcController::RpcController() : failed_(false), canceled_(false) {
}

RpcController::~RpcController() {
}

void RpcController::Reset() {
  // TODO(yeshunping) : Implement it
}

bool RpcController::Failed() const {
  return failed_;
}

string RpcController::ErrorText() const {
  return error_reason_;
}

void RpcController::StartCancel() {
  canceled_ = true;
}

void RpcController::SetFailed(const std::string& reason) {
  failed_ = true;
  error_reason_ = reason;
}

bool RpcController::IsCanceled() const {
  return canceled_;
}

void RpcController::NotifyOnCancel(google::protobuf::Closure* callback) {
  // TODO(yeshunping) : Implement it
}
}  // namespace poppy
