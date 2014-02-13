// Copyright 2014. All Rights Reserved.
// Author: yeshunping@gmail.com (Shunping Ye)

//  A trivial static http webserver using Libevent's evhttp.

#include <stdio.h>
#include <signal.h>

#include "base/logging.h"
#include "base/callback.h"
#include "base/flags.h"
#include "base/binary_version.h"
#include "protorpc/http_server/http_request.h"
#include "protorpc/http_server/http_response.h"
#include "protorpc/http_server/http_server.h"
#include "protorpc/http_server/event_loop.h"
#include "protorpc/http_server/debug_util.h"
#include "protorpc/rpc_server/rpc_server.h"
#include "thirdparty/event/include/event2/http.h"
#include "thirdparty/event/include/event2/buffer.h"

DEFINE_int32(listen_port, 10012, "");

int main(int argc, char **argv) {
  base::SetupBinaryVersion();
  google::ParseCommandLineFlags(&argc, &argv, false);
  protorpc::EventLoop event_loop;
  protorpc::RpcServer http_server(argv[0], &event_loop, FLAGS_listen_port);

  http_server.Start();
  event_loop.Loop();

  return 0;
}
