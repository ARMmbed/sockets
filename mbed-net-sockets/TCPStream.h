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
  TCPStream(handler_t defaultHandler);
  ~TCPStream();

public:
  socket_error_t connect(SocketAddr *address, const uint16_t port, handler_t onConnect);
  void onDisconnect(handler_t h) {_onDisconnect = h;}
public:
  socket_error_t start_send(void *buf, const size_t len, const handler_t &sendHandler, const uint32_t flags = 0);
  void flush_send();
  void stop_send();

  socket_error_t start_recv(handler_t &recvHandler);
  void stop_recv();

  // virtual SocketBuffer * getBuffer(size_t len);
  // virtual SocketBuffer * getBuffer(void *buf, size_t len);
  // virtual SocketBuffer * getBuffer(struct socket_buffer *sb);

  SocketBuffer & getRxBuf() {return _rxBuf;}
protected:
  void _eventHandler(void*);
  void onDNS(socket_event_t *e);

protected:
  handler_t _onSent;
  handler_t _onReceive;
  handler_t _onConnect;
  handler_t _onDisconnect;
  uint16_t _port;

protected:
  SocketBuffer *_txBuf;
  SocketBuffer _rxBuf;
};

#endif // MBED_TCPStream_H
