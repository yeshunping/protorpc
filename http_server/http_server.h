// Copyright 2014. All Rights Reserved.
// Author: yeshunping@gmail.com (Shunping Ye)

#include "base/callback.h"

struct evhttp_request;
struct evhttp;
struct evhttp_bound_socket;

namespace protorpc {
class HttpRequest;
class HttpResponse;
class EventLoop;

// TODO(yeshunping) : Refactor http server later, drop libevent
class HttpServer {
 public:
  HttpServer(EventLoop* event_loop, int listen_port);
  ~HttpServer();

  void SetHttpHandler(const string& path, base::ResultCallback2<bool, HttpRequest*, HttpResponse*>* callback);
  void Start();

  const string& uri_root() const {
    return uri_root_;
  }
 private:
  void ShowBindInfo(struct evhttp_bound_socket *handle);

  struct evhttp *http_;
  int listen_port_;
  string uri_root_;
  DISALLOW_COPY_AND_ASSIGN(HttpServer);
};
}  // namespace protorpc
