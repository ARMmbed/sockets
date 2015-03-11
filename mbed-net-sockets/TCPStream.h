#ifndef MBED_TCPStream_H
#define MBED_TCPStream_H
/*
 * PackageLicenseDeclared: Apache-2.0
 * Copyright 2015 ARM Holdings PLC
 */
 #include <stddef.h>
 #include <stdint.h>
 #include <Ticker.h>
 #include "socket_types.h"
 #include "TCPAsynch.h"
 #include "buffer.h"

class TCPStream: public TCPAsynch {
public:
  /* Socket Creation API */
  TCPStream(const socket_stack_t stack);
  ~TCPStream();

public:
  socket_error_t connect(const SocketAddr *address, const uint16_t port, const handler_t onConnect);
  void onDisconnect(const handler_t h) {_onDisconnect = h;}



protected:
  void _eventHandler(struct socket_event *ev);

protected:
  handler_t _onConnect;
  handler_t _onDisconnect;
  uint16_t _port;
};

#endif // MBED_TCPStream_H
