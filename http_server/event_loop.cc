// Copyright 2014. All Rights Reserved.
// Author: yeshunping@gmail.com (Shunping Ye)

#include "protorpc/http_server/event_loop.h"

#include "base/logging.h"
#include "thirdparty/event/include/event2/event.h"

namespace protorpc {

EventLoop::EventLoop() {
  base_ = event_base_new();
  CHECK(base_) << "Couldn't create an event_base: exiting";
}

EventLoop::~EventLoop() {
}

void EventLoop::Loop() {
  event_base_dispatch(base_);
}

}  // namespace protorpc
