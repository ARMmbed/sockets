/*
 * PackageLicenseDeclared: Apache-2.0
 * Copyright 2015 ARM Holdings PLC
 */
#include "socket_types.h"
#include "UDPaSocket.h"

#include "socket_api.h"
#include "socket_buffer.h"
#include "SocketBuffer.h"

UDPaSocket::UDPaSocket(socket_stack_t stack):
/* Store the default handler */
    aSocket(stack)
{
}

UDPaSocket::~UDPaSocket()
{
}

socket_error_t
UDPaSocket::bind(SocketAddr *address, uint16_t port)
{
    socket_error_t err = _socket.api->bind(&_socket, address->getAddr(), port);
    return err;
}

socket_error_t UDPaSocket::connect(SocketAddr *address, uint16_t port)
{
    socket_error_t err = _socket.api->connect(&_socket, address->getAddr(), port);
    return err;
}
