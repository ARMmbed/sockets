#ifndef MBED_TCP_ASYNCH_H
#define MBED_TCP_ASYNCH_H

#include "aSocket.h"
#include "Ticker.h"
#include "socket_api.h"

class TCPAsynch: public aSocket {
public:
    socket_error_t bind(const char *addr, uint16_t port);
    socket_error_t bind(const SocketAddr *addr, uint16_t port);
protected:
  TCPAsynch(handler_t defaultHandler, const socket_stack_t stack);
  ~TCPAsynch();

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
