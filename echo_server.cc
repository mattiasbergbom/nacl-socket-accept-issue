// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "echo_server.h"

#include <string.h>
#include <sstream>

#include "ppapi/c/pp_errors.h"
#include "ppapi/cpp/var.h"
#include "ppapi/utility/completion_callback_factory.h"

#ifdef WIN32
#undef PostMessage
#endif

int start_tcp_server(pp::Instance* instance, int port);

namespace {
  pthread_t g_server_thread;
  uint16_t g_port;
  pp::Instance* g_instance = NULL;
  void* ServerThread(void* user_data) {
      start_tcp_server(g_instance,g_port);
      return 0;
  }
  
}

void EchoServer::Start(uint16_t port) {
    g_port = port;
    g_instance = instance_;
    pthread_create(&g_server_thread, NULL, &ServerThread, NULL);
}
