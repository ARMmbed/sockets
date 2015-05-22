/*
 * PackageLicenseDeclared: Apache-2.0
 * Copyright 2015 ARM Holdings PLC
 */
#include <mbed-net-sockets/TCPListener.h>
#include <mbed-net-sockets/TCPStream.h>
#include <mbed-net-socket-abstract/socket_api.h>

using namespace mbed;


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
TCPStream * TCPListener::accept(void *new_impl)
{
    struct socket new_socket = _socket;
    new_socket.impl = new_impl;
    new_socket.stack = _socket.stack;
    new_socket.family = _socket.family;
    return new TCPStream(&new_socket);
}


void TCPListener::_eventHandler(struct socket_event *ev)
{
    switch(ev->event) {
    case SOCKET_EVENT_RX_ERROR:
    case SOCKET_EVENT_TX_ERROR:
    case SOCKET_EVENT_ERROR:
        if (_onError)
            _onError(ev->i.e);
        break;
    case SOCKET_EVENT_RX_DONE:
    case SOCKET_EVENT_TX_DONE:
    case SOCKET_EVENT_CONNECT:
    case SOCKET_EVENT_DISCONNECT:
        if(_onError)
            _onError(SOCKET_ERROR_UNIMPLEMENTED);
        break;
    case SOCKET_EVENT_DNS:
        if (_onDNS)
            _onDNS(SOCKET_ERROR_NONE);
        break;
    case SOCKET_EVENT_ACCEPT:
        if (_onIncomming)
            _onIncomming(SOCKET_ERROR_NONE);
        break;
    case SOCKET_EVENT_NONE:
    default:
        break;
    }
}
