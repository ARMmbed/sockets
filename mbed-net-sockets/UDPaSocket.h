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
#include "SocketBuffer.h"

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
    UDPaSocket(handler_t defaultHandler, const socket_allocator_t *alloc);

    /**
     * Specify the listening port and address for the (server) socket
     * This is not necessary for a client socket
     * @param[in] address The internet address to bind (may be 0.0.0.0/INET_ADDR_ANY)
     * @param[in] port The port to listen on
     * @return Returns 0 on success or an error code on failure
     */
    socket_error_t bind(SocketAddr *address, uint16_t port);
    ~UDPaSocket();
public:
    /* Socket Communication API */
    /**
    * Sends a datagram to the specified address.
    * The sendHandler will be called when the send completes.
    * This method is not zero-copy.
    * @param[in] address The destination address
    * @param[in] port The destination port
    * @param[in] buf The raw buffer to send
    * @param[in] length The length, in bytes, of the raw buffer
    * @param[in] flags The flags to use for the send.  Supported flags are: MSG_EOR, MSG_OOB.
    * @param[in] sendHandler The handler to call when the send is complete.  May be the Null Handler.
    * @return Returns an error code or 0 if the handler was installed and the transfer was started successfully.
    */
    socket_error_t start_send_to(SocketAddr *address, const uint16_t port, const void *buffer, const size_t length, const int flags, handler_t &sendHandler);
    /**
    * Sends a datagram to the specified address.
    * The sendHandler will be called when the send completes.
    * This method is zero-copy.
    * @param[in] address The destination address
    * @param[in] port The destination port
    * @param[in] SocketBuffer The socket buffer object to send
    * @param[in] flags The flags to use for the send.  Supported flags are: MSG_EOR, MSG_OOB.
    * @param[in] sendHandler The handler to call when the send is complete.  May be the Null Handler.
    * @return Returns an error code or 0 if the handler was installed and the transfer was started successfully.
    */
    socket_error_t start_send_to(SocketAddr *address, const uint16_t port, SocketBuffer *buffer, const int flags, handler_t &sendHandler);
    socket_error_t start_send_to(SocketBuffer *buffer);

    /**
     * Receives on the UDP socket with no address filtering
     * @param[in] buffer The buffer to receive into
     * @param[in] flags The flags used for receive. Supported flags are: MSG_OOB
     * @param[in] recvHandler The handler to call when the receive is complete.  May NOT be the Null handler.
     * @return Returns an error code or 0 if the handler was installed and the transfer was started successfully.
     */
    socket_error_t start_recv(handler_t &recvHandler);

    // TODO: (CThunk upgrade/Alpha3)
    // socket_event_t * getEvent(){
    //     return aSocket::getEvent();
    // }

protected:
    void _eventHandler(void*);
protected:
    handler_t _recvHandler;
    SocketBuffer *_txBuf;
};

#endif // MBED_UDPaSocket_H
