// Copyright 2014. All Rights Reserved.
// Author: yeshunping@gmail.com (Shunping Ye)

#include "protorpc/examples/echo_service.pb.h"
#include "protorpc/rpc_server/rpc_server.h"
#include "protorpc/http_server/event_loop.h"

#include "base/flags.h"
#include "base/logging.h"
#include "base/binary_version.h"

DEFINE_int32(listen_port, 10000, "the address server listen on.");

namespace examples {

class EchoServerImpl : public EchoServer {
 public:
  EchoServerImpl() {
  }
  virtual ~EchoServerImpl() {
  }

 private:
  virtual void Echo(
      google::protobuf::RpcController* controller,
      const EchoRequest* request,
      EchoResponse* response,
      google::protobuf::Closure* done) {
    LOG(INFO) << "An empty parameter request, user:" << request->user() << ", message:" << request->message();
    response->set_user(request->user());
    response->set_message(request->message());
    done->Run();
  }
};

}  // end namespace examples

int main(int argc, char** argv) {
  base::SetupBinaryVersion();
  google::ParseCommandLineFlags(&argc, &argv, true);

  protorpc::EventLoop event_loop;
  protorpc::RpcServer rpc_server(&event_loop, FLAGS_listen_port);
  examples::EchoServerImpl* echo_service =
      new examples::EchoServerImpl();
  rpc_server.RegisterService(echo_service);

  rpc_server.Start();
  event_loop.Loop();

  return EXIT_SUCCESS;
}
