/*
 * PackageLicenseDeclared: Apache-2.0
 * Copyright 2015 ARM Holdings PLC
 */
#include <mbed-net-sockets/UDPSocket.h>
#include <mbed-net-socket-abstract/socket_api.h>

using namespace mbed;
UDPSocket::UDPSocket(socket_stack_t stack):
/* Store the default handler */
    Socket(stack)
{
}

UDPSocket::~UDPSocket()
{
}

socket_error_t UDPSocket::connect(const SocketAddr *address, const uint16_t port)
{
    socket_error_t err = _socket.api->connect(&_socket, address->getAddr(), port);
    return err;
}
