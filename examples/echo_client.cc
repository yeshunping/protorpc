// Copyright 2014. All Rights Reserved.
// Author: yeshunping@gmail.com (Shunping Ye)

#include "protorpc/examples/echo_service.pb.h"
#include "protorpc/rpc_server/rpc_channel.h"
#include "protorpc/rpc_client/rpc_client.h"
#include "protorpc/rpc_server/rpc_controller.h"

#include "base/flags.h"
#include "base/logging.h"
#include "base/binary_version.h"

DEFINE_string(server_host, "127.0.0.1", "the server host");
DEFINE_int32(server_port, 10000, "the server port listen on.");

int main(int argc, char** argv) {
  base::SetupBinaryVersion();
  google::ParseCommandLineFlags(&argc, &argv, true);

  protorpc::RpcClient rpc_client;
  protorpc::RpcChannel rpc_channel(&rpc_client, FLAGS_server_host, FLAGS_server_port);
  examples::EchoServer::Stub echo_client(&rpc_channel);

  protorpc::RpcController rpc_controller;

  examples::EchoRequest request;
  request.set_user("test_user");
  request.set_message("test_message");

  examples::EchoResponse response;
  LOG(INFO)<< "sending request";
  echo_client.Echo(&rpc_controller, &request, &response, NULL);

  if (rpc_controller.Failed()) {
    LOG(INFO)<< "failed: " << rpc_controller.ErrorText();
  } else {
    LOG(INFO) << "response info:\n" << response.Utf8DebugString();
  }

  return 0;
}
