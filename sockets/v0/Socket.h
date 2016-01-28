/*
 * PackageLicenseDeclared: Apache-2.0
 * Copyright (c) 2015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __SOCKETS_V0_SOCKET_H__
#define __SOCKETS_V0_SOCKET_H__


#include <stddef.h>
#include <stdint.h>
#include "mbed-drivers/mbed.h"
#include "core-util/FunctionPointer.h"
#include "mbed-drivers/CThunk.h"
#include "sal/socket_types.h"
#include "SocketAddr.h"

namespace mbed {
namespace Sockets {
namespace v0 {

/**
 * \brief Socket implements most of the interfaces required for sockets.
 * Socket is a pure virtual class; it should never be instantiated directly, but it provides
 * common functionality for derived classes.
 */
class Socket {
public:
    typedef mbed::util::FunctionPointer3<void, Socket *, struct socket_addr, const char *> DNSHandler_t;
    typedef mbed::util::FunctionPointer2<void, Socket *, socket_error_t> ErrorHandler_t;
    typedef mbed::util::FunctionPointer1<void, Socket *> ReadableHandler_t;
    typedef mbed::util::FunctionPointer2<void, Socket *, uint16_t> SentHandler_t;
protected:
    /**
     * Socket constructor
     * Initializes the Socket object.  Initializes the underlying struct socket.  Does not instantiate
     * an underlying network stack socket.
     * Since it is somewhat awkward to provide the network stack, a future change will provide
     * a way to pass the network interface to the socket constructor, which will extract the stack from
     * the interface.
     * @param[in] stack The network stack to use for this socket.
     */
    Socket(const socket_stack_t stack);

    /**
     * Socket destructor
     * Frees the underlying socket implementation.
     */
    virtual ~Socket();
public:
    /**
     * Start the process of resolving a domain name.
     * If the input is a text IP address, an event is queued immediately; otherwise, onDNS is
     * queued as soon as DNS is resolved.
     * @param[in] address The domain name to resolve
     * @param[in] onDNS The handler to call when the name is resolved
     * @retval SOCKET_ERROR_NONE on success
     * @retval SOCKET_ERROR_NULL_PTR if address is NULL
     * @return Error code on failure
     */
    virtual socket_error_t resolve(const char* address, const DNSHandler_t &onDNS);

    /**
     * Open the socket.
     * Instantiates and initializes the underlying socket. Receive is started immediately after
     * the socket is opened.
     * @param[in] af Address family (SOCKET_AF_INET4 or SOCKET_AF_INET6), currently only IPv4 is supported
     * @param[in] pf Protocol family (SOCKET_DGRAM or SOCKET_STREAM)
     * @retval SOCKET_ERROR_NONE on success
     * @retval SOCKET_ERROR_BAD_STACK if there is no valid underlying network stack
     * @retval SOCKET_ERROR_BAD_FAMILY if an invalid Address or Protocol family is supplied
     * @return Error code on failure
     */
    virtual socket_error_t open(const socket_address_family_t af, const socket_proto_family_t pf);

    /**
     * Binds the socket's local address and IP.
     * 0.0.0.0 is accepted as a local address if only the port is meant to be bound.
     * The behaviour of bind("0.0.0.0",...) is undefined where two or more stacks are in use.
     * Specifying a port value of 0, will instruct the stack to allocate an available port
     * automatically.
     *
     * @param[in] address The string representation of the address to bind
     * @param[in] port The local port to bind
     * @retval SOCKET_ERROR_NONE on success
     * @retval SOCKET_ERROR_NULL_PTR if the socket has not been opened or the address is NULL
     * @return Error code on failure
     */
    virtual socket_error_t bind(const char *address, const uint16_t port);

    /**
     * bind(const SocketAddr *, const uint16_t) is the same as bind(const char *, const uint16_t),
     * except that the address passed in is a SocketAddr.
     * @param[in] address The address to bind
     * @param[in] port The local port to bind
     * @retval SOCKET_ERROR_NONE on success
     * @retval SOCKET_ERROR_NULL_PTR if the supplied address is NULL
     * @return Error code on failure
     */
    virtual socket_error_t bind(const SocketAddr *address, const uint16_t port);

    /**
     * Set the error handler.
     * Errors are ignored if onError is not set.
     * @param[in] onError
     */
    virtual void setOnError(const ErrorHandler_t &onError);

    /**
     * Set the received data handler
     * Received data is queued until it is read using recv or recv_from.
     * @param[in] onReadable the handler to use for receive events
     */
    virtual void setOnReadable(const ReadableHandler_t &onReadable);

    /**
     * Receive a message
     * @param[out] buf The buffer to fill
     * @param[in,out] len A pointer to the size of the receive buffer.  Sets the maximum number of bytes
     * to read but is updated with the actual number of bytes copied on success.  len is not changed on
     * failure
     * @retval SOCKET_ERROR_NONE on success
     * @retval SOCKET_ERROR_NULL_PTR if the socket has not been opened, buf is NULL or len is NULL
     * @return Error code on failure
     */
    virtual socket_error_t recv(void * buf, size_t *len);

    /**
     * Receive a message with the sender address and port
     * This API is not valid for SOCK_STREAM
     * @param[out] buf The buffer to fill
     * @param[in,out] len A pointer to the size of the receive buffer.  Sets the maximum number of bytes
     * to read but is updated with the actual number of bytes copied on success.  len is not changed on
     * failure
     * @param[out] remote_addr Pointer to an address structure to fill with the sender address
     * @param[out] remote_port Pointer to a uint16_t to fill with the sender port
     * @retval SOCKET_ERROR_NONE on success
     * @retval SOCKET_ERROR_NULL_PTR if the socket has not been opened or any of the pointer arguments
     *                               are NULL
     * @return Error code on failure
     */
    virtual socket_error_t recv_from(void * buf, size_t *len, SocketAddr *remote_addr, uint16_t *remote_port);

    /**
     * Set the onSent handler.
     * The exact moment this handler is called varies from implementation to implementation.
     * On LwIP, onSent is called when the remote host ACK's data in TCP sockets, or when the message enters
     * the network stack in UDP sockets.
     * @param[in] onSent The handler to call when a send completes
     */
    virtual void setOnSent(const SentHandler_t &onSent);

    /**
     * Send a message
     * Sends a message over an open connection.  This call is valid for UDP sockets, provided that connect()
     * has been called.
     * @param[in] buf The payload to send
     * @param[in] len The size of the payload
     * @retval SOCKET_ERROR_NONE on success
     * @retval SOCKET_ERROR_NULL_PTR if the socket has not been opened or buf is NULL
     * @return Error code on failure
     */
    virtual socket_error_t send(const void * buf, const size_t len);

    /**
     * Send a message to a specific address and port
     * This API is not valid for SOCK_STREAM
     * @param[in] buf The payload to send
     * @param[in] len The size of the payload
     * @param[in] address The address to send to
     * @param[in] port The remote port to send to
     * @retval SOCKET_ERROR_NONE on success
     * @retval SOCKET_ERROR_NULL_PTR if the socket has not been opened, buf is NULL or the
     *                               remote_addr is NULL
     * @return Error code on failure
     */
    virtual socket_error_t send_to(const void * buf, const size_t len, const SocketAddr *remote_addr, uint16_t remote_port);

    /**
     * Shuts down a socket.
     * Sending and receiving are no longer possible after close() is called.
     * The socket is not deallocated on close.  A socket must not be reopened, it should be
     * destroyed (either with delete, or by going out of scope) after calling close.
     * Calling open on a closed socket can result in a memory leak.
     * @retval SOCKET_ERROR_NONE on success
     * @retval SOCKET_ERROR_NULL_PTR if the socket has not been opened
     * @return Error code on failure
     */
    virtual socket_error_t close();

    /**
     * Error checking utility
     * Generates an event on error, does nothing on SOCKET_ERROR_NONE
     * @param[in] err the error code to check
     * @return false if err is SOCKET_ERROR_NONE, true otherwise
     */
    virtual bool error_check(socket_error_t err);

    /**
     * Checks the socket status to determine whether it is still connected.
     * @return true if the socket is connected, false if it is not
     */
    virtual bool isConnected() const;

    /**
     * Get the local address of the socket if bound.
     * Populates the SocketAddr object with the local address
     *
     * @param[out] addr a pointer to a SocketAddr object
     * @retval SOCKET_ERROR_NONE on success
     * @retval SOCKET_ERROR_NULL_PTR if the socket has not been opened or the addr is NULL
     * @retval SOCKET_ERROR_NOT_BOUND if the socket has not been bound
     * @return Error code on failure
     */
    virtual socket_error_t getLocalAddr(SocketAddr *addr) const;

    /**
     * Get the local port of the socket if bound.
     * Populates the uint16_t object with the local port
     *
     * @param[out] port a pointer to a uint16_t
     * @retval SOCKET_ERROR_NONE on success
     * @retval SOCKET_ERROR_NULL_PTR if the socket has not been opened or the port is NULL
     * @retval SOCKET_ERROR_NOT_BOUND if the socket has not been bound
     * @return Error code on failure
     */
    virtual socket_error_t getLocalPort(uint16_t *port) const;

    /**
     * Get the remote address of the socket if connected.
     * Populates the SocketAddr object with the remote address
     *
     * @param[out] addr a pointer to a SocketAddr object
     * @retval SOCKET_ERROR_NONE on success
     * @retval SOCKET_ERROR_NULL_PTR if the socket has not been opened or the addr is NULL
     * @retval SOCKET_ERROR_NO_CONNECTION if the socket has not been connected
     * @return Error code on failure
     */
    virtual socket_error_t getRemoteAddr(SocketAddr *addr) const;

    /**
     * Get the remote port of the socket if connected.
     * Populates the uint16_t object with the remote port
     *
     * @param[out] port a pointer to a uint16_t
     * @retval SOCKET_ERROR_NONE on success
     * @retval SOCKET_ERROR_NULL_PTR if the socket has not been opened or the port is NULL
     * @retval SOCKET_ERROR_NO_CONNECTION if the socket has not been connected
     * @return Error code on failure
     */
    virtual socket_error_t getRemotePort(uint16_t *port) const;

#if 0 // not implemented yet
    static long ntohl(long);
    static short ntohs(short);
    static long long ntohll(long long);
#endif

protected:
    /** \internal
     * The internal event handler
     * @param[in] ev The event to handle
     */
    virtual void _eventHandler(struct socket_event *ev);

protected:
    /** Function pointer to the user-supplied DNS response handling function
     * \internal
     *  This function pointer is called every time a DNS response is received.
     *  If the function pointer is false when cast to a boolean, DNS events are discarded.
     */
    DNSHandler_t      _onDNS;
    /** Function pointer to the user-supplied error handling function
     * \internal
     *  This function pointer is called when an error condition is encountered in a part
     *  of the code which does not have a direct call path back to user-supplied code
     *  If NULL, error handling is discarded.
     */
    ErrorHandler_t    _onError;
    /** Function pointer to the user-supplied data available handling function

     *  This function pointer is called when data is available on the socket. It is called once for
     *  each event generated by the socket layer, so partial reads can leave data available on the
     *  socket and greedy reads can cause extra events with no data to read.
     *  Suggested handling is to use greedy reads and terminate early in events with no */
    ReadableHandler_t _onReadable; ///< Function pointer to the user-supplied socket readable function
    SentHandler_t     _onSent;

    CThunk<Socket> _irq;
public:
    struct socket _socket;
private:
    socket_event_t *_event;
    /**
     * Internal event handler.
     * @param[in] arg
     */
    void _nvEventHandler(void * arg);
};
} // namespace v0
} // namespace Sockets
} // namespace mbed
#endif // __SOCKETS_V0_SOCKET_H__
