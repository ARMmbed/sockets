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
    /* Socket Creation API */
    /**
     * UDP socket constructor.
     * Requires a port because the underlying UDP socket is allocated and bound at creation time. If an error occurs
     * during construction, the defaultHandler will be called.
     * @param[in] port The local port to listen on
     * @param[in] defaultHandler The default handler to use if no transmissions have been initiated or on error
     */
    UDPaSocket(socket_stack_t stack);

    socket_error_t inline open(const socket_address_family_t af)
    {
    	return aSocket::open(af,SOCKET_DGRAM);
    }

    /**
     * Specify the listening port and address for the (server) socket
     * This is not necessary for a client socket
     * @param[in] address The internet address to bind (may be 0.0.0.0/INET_ADDR_ANY)
     * @param[in] port The port to listen on
     * @return Returns 0 on success or an error code on failure
     */
    socket_error_t connect(const SocketAddr *address, const uint16_t port);
    ~UDPaSocket();

};

#endif // MBED_UDPaSocket_H
