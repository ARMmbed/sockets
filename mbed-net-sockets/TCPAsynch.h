#ifndef MBED_TCP_ASYNCH_H
#define MBED_TCP_ASYNCH_H

#include "aSocket.h"
#include "Ticker.h"
#include "socket_api.h"

class TCPAsynch: public aSocket {
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

#endif // MBED_TCP_ASYNCH_H
