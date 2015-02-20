/*
 * PackageLicenseDeclared: Apache-2.0
 * Copyright 2015 ARM Holdings PLC
 */
#include <new>
#include "TCPListener.h"
#include "socket_api.h"

TCPListener::TCPListener(handler_t defaultHandler, const socket_stack_t stack) :
    TCPAsynch(defaultHandler, stack),
{

}
TCPListener::~TCPListener()
{
    stop_listening();
}

socket_error_t TCPListener::bind(struct socket_addr *address, uint16_t port)
{
    return _socket.api->bind(&_socket, address, port);
}

socket_error_t TCPListener::start_listening(handler_t &listenHandler, uint backlog)
{
    _onIncomming = &listenHandler;
    socket_error_t err = _socket.api->start_listen(&_socket);
    return err;
}
socket_error_t TCPListener::stop_listening()
{
    socket_error_t err = _socket.api->stop_listen(&_socket);
    return err;
}
TCPStream * TCPListener::accept(handler_t defaultHandler)
{
    return new TCPStream(defaultHandler, _socket.stack);
}

/* TCP Server Accept methods
 * 1) Use new to create a socket.
 * 2) Pass in an allocator to create a socket
 * 3) Pass a pool of memory to the TCP Server on creation and allocate out of the pool
 *
 * Problems: TCP Stream events need to be routed through TCP server so
 * that onDisconnect can tear down the TCP Stream after the onDisconnect handler
 * has been called
