// Copyright 2014. All Rights Reserved.
// Author: yeshunping@gmail.com (Shunping Ye)

#ifndef PROTORPC_HTTP_SERVER_HTTP_SERVER_H_
#define PROTORPC_HTTP_SERVER_HTTP_SERVER_H_

#include "base/callback.h"
#include "base/hash_tables.h"

struct evhttp_request;
struct evhttp;
struct evhttp_bound_socket;

namespace protorpc {
class HttpRequest;
class HttpResponse;
class EventLoop;
class HttpHandler;

// TODO(yeshunping) : Refactor http server later, drop libevent
class HttpServer {
 public:
  HttpServer(EventLoop* event_loop, int listen_port);
  virtual ~HttpServer();

  bool RegisterHttpHandler(const string& path, HttpHandler* handler);
  void Start();

  const string& uri_root() const {
    return uri_root_;
  }
 private:
  void ShowBindInfo(struct evhttp_bound_socket *handle);

  struct evhttp *http_;
  int listen_port_;
  string uri_root_;
  hash_map<std::string, HttpHandler*> handlers_;
  DISALLOW_COPY_AND_ASSIGN(HttpServer);
};
}  // namespace protorpc
#endif  // PROTORPC_HTTP_SERVER_HTTP_SERVER_H_
