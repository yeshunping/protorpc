// Copyright 2014. All Rights Reserved.
// Author: yeshunping@gmail.com (Shunping Ye)

#include "protorpc/rpc_server/rpc_channel.h"

#include "base/flags.h"
#include "base/logging.h"
#include "base/string_util.h"
#include "protorpc/rpc_client/rpc_client.h"
#include "thirdparty/protobuf/include/google/protobuf/descriptor.h"
#include "thirdparty/protobuf/include/google/protobuf/message.h"

namespace protorpc {

// TODO(yeshunping) : Move to header file for both server/client
static const char kMethodNameHeader[] = "Protorpc_Method_Name";
const char kRpcServiceHttpPath[] = "/__rpc__";

RpcChannel::RpcChannel(protorpc::RpcClient* client,
                       const string& host,
                       int port)
    : client_(client),
      host_(host),
      port_(port) {
}

RpcChannel::~RpcChannel() {
}

void RpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                            google::protobuf::RpcController* controller,
                            const google::protobuf::Message* request,
                            google::protobuf::Message* response,
                            google::protobuf::Closure* done) {
  VLOG(2) << "CallMethod called, request :\n" << request->DebugString();
  //  Set service name and method name in http header
  client_->SetHttpMethod("POST");

  // FIXME(yeshunping) : name or full_name ?
  string method_name = method->full_name();
  client_->AddHeader(kMethodNameHeader, method_name);

  // TODO(yeshunping) : compress request data
  //  serialize message into binary string, then set as http body
  string data = request->SerializeAsString();
  client_->SetPostData(data);

  string url = StringPrintf("http://%s:%d%s", host_.c_str(), port_, kRpcServiceHttpPath);
  string response_data;
  // TODO(yeshunping) : Support asyn mode
  if (!client_->FetchUrl(url, &response_data)) {
    LOG(ERROR) << "Fail to send request:" << url;
    controller->SetFailed(client_->GetErrorInfo());
    done->Run();
    return;
  }
  long code = 0;
  client_->GetResponseCode(&code);
  LOG(INFO) << "request send, response code:" << code;
  // TODO(yeshunping) : Uncompress data,
  response->ParseFromString(response_data);
  if (done) {
    done->Run();
  }
}
}  // namespace poppy
