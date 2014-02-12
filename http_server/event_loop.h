// Copyright 2014. All Rights Reserved.
// Author: yeshunping@gmail.com (Shunping Ye)

#ifndef PROTORPC_HTTP_SERVER_EVENT_LOOP_H_
#define PROTORPC_HTTP_SERVER_EVENT_LOOP_H_

#include "base/basictypes.h"

struct event_base;

namespace protorpc {
class EventLoop {
 public:
  EventLoop();
  ~EventLoop();

  struct event_base* base() {
    return base_;
  }
  void Loop();
 private:
  struct event_base* base_;
  DISALLOW_COPY_AND_ASSIGN(EventLoop);
};
}  // namespace protorpc

#endif  // PROTORPC_HTTP_SERVER_EVENT_LOOP_H_
