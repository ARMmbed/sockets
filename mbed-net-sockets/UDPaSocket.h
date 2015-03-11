/*
 * PackageLicenseDeclared: Apache-2.0
 * Copyright 2015 ARM Holdings PLC
 */
#ifndef MBED_UDPaSocket_H
#define MBED_UDPaSocket_H

#include <stddef.h>
#include <stdint.h>
#include "socket_types.h"
#include "aSocket.h"

/* UDP socket class */
class UDPaSocket: public aSocket {
public:
    /**
     * UDP socket constructor.
     * Does not allocate an underlying UDP Socket instance.
	 * @param[in] stack The network stack to use for this socket.
     */
    UDPaSocket(socket_stack_t stack);
    /**
     * UDP Socket destructor
     */
    ~UDPaSocket();
    /**
     * Open a UDP socket
     * Instantiates and initializes the underlying socket. Receive is started immediately after
     * the socket is opened.
     * @param[in] af Address family (SOCKET_AF_INET4 or SOCKET_AF_INET6), currently only IPv4 is supported
     * @return SOCKET_ERROR_NONE on success, or an error code on failure
     */
    socket_error_t inline open(const socket_address_family_t af)
    {
    	return aSocket::open(af,SOCKET_DGRAM);
    }
    /**
     * Connect to a remote host.
     * This is an internal configuration API only.  No network traffic is generated.
     * @param[in] address The remote host to connect to
     * @param[in] port The remote port to connect to
     * @return SOCKET_ERROR_NONE on success, or an error code on failure
     */
    socket_error_t connect(const SocketAddr *address, const uint16_t port);
};

#endif // MBED_UDPaSocket_H
