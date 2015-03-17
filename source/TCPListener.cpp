/*
 * PackageLicenseDeclared: Apache-2.0
 * Copyright 2015 ARM Holdings PLC
 */
#include <mbed-net-sockets/TCPListener.h>
#include <mbed-net-sockets/TCPStream.h>
#include <mbed-net-socket-abstract/socket_api.h>

TCPListener::TCPListener(const socket_stack_t stack) :
    TCPAsynch(stack), _onIncomming(NULL)
{
}
TCPListener::~TCPListener()
{
    stop_listening();
}

socket_error_t TCPListener::start_listening(handler_t listenHandler, uint32_t backlog)
{
    _onIncomming = listenHandler;
    socket_error_t err = _socket.api->start_listen(&_socket, backlog);
    return err;
}
socket_error_t TCPListener::stop_listening()
{
    socket_error_t err = _socket.api->stop_listen(&_socket);
    return err;
}
TCPStream * TCPListener::accept(struct socket *new_socket)
{
    return new TCPStream(new_socket);
}
