/*
 * PackageLicenseDeclared: Apache-2.0
 * Copyright 2015 ARM Holdings PLC
 */
#ifndef __MBED_NET_SOCKETS_TCP_ASYNCH__
#define __MBED_NET_SOCKETS_TCP_ASYNCH__

#include <mbed-net-sockets/Socket.h>
#include <mbed-net-socket-abstract/socket_api.h>

#include "Ticker.h"

namespace mbed {
class TCPAsynch: public Socket {
protected:
  TCPAsynch(const socket_stack_t stack);
  ~TCPAsynch();

public:
  socket_error_t open(const socket_address_family_t af);
protected:
  static Ticker _ticker;
  static handler_t _tick_handler;
  // uintptr_t is used to guarantee that there will always be a large enough
  // counter to avoid overflows. Memory allocation will always fail before
  // counter overflow if the counter is the same size as the pointer type and
  // sizeof(TCPAsynch) > 0
  static uintptr_t _TCPSockets;
};

}; //namespace mbed
#endif // __MBED_NET_SOCKETS_TCP_ASYNCH__
