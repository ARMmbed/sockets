#ifndef MBED_TCPListener_H
#define MBED_TCPListener_H
/*
 * PackageLicenseDeclared: Apache-2.0
 * Copyright 2015 ARM Holdings PLC
 */
#include <stddef.h>
#include <stdint.h>
#include "socket_types.h"
#include "TCPAsynch.h"
#include "buffer.h"
#include "CThunk.h"

class TCPListener: protected TCPAsynch{
public:
  /* Socket Creation API */
  TCPListener(handler_t defaultHandler, const socket_stack_t stack)
  ~TCPaSocket();

  socket_error_t bind(struct socket_addr *address, uint16_t port);

  socket_error_t start_listening(handler_t &listenHandler, uint backlog);
  socket_error_t stop_listening();

  TCPStream * accept(handler_t defaultHandler, struct socket *new_socket);

protected:
  void _eventHandler(void*);

protected:
    CThunk<TCPListener> _onAccept;
}

#endif // MBED_TCPListener_H
