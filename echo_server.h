// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ECHO_SERVER_H_
#define ECHO_SERVER_H_

#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/tcp_socket.h"
#include "ppapi/utility/completion_callback_factory.h"

#include <nacl_io/nacl_io.h>

static const int kBufferSize = 1024;

// Simple "echo" server based on a listening pp::TCPSocket.
// This server handles just one connection at a time and will
// echo back whatever bytes get sent to it.
class EchoServer {
  public:
    EchoServer(pp::Instance* instance, uint16_t port)
        : instance_(instance)
    {
        nacl_io_init_ppapi( instance_->pp_instance(),
                            pp::Module::Get()->get_browser_interface() );
        Start(port);
    }
  protected:
    void Start(uint16_t port);

    pp::Instance* instance_;
};

#endif  // ECHO_SERVER_H_
