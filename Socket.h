
#ifndef MBED_SOCKET_H
#define MBED_SOCKET_H
#include <stddef.h>
#include <stdint.h>


typedef struct {
    // TBD
} event_t;
typedef enum {

} socket_error_t;
typedef enum {

} socket_flags_t;

// TODO: The type of handler_t is TBD.
// NOTE: Since handlers are passed using C++ references, a global null handler will be provided so that empty handlers
// are provided for.  Overriding the null handler is likely to be useful for debugging.
/*
 * std::function, in combination with std::bind appear to be the ideal solution for handler_t, however
 * std::bind, is not supported by all our targets. We might be able to work around this by porting
 * boost::bind to mbed
 */
//typedef std::function<void(event_t*)> handler_t;
/*
 * We already have a feature in mbed for handling function pointers.
 */
//#include "FunctionPointer.h"
//typedef FunctionPointer handler_t
/*
 * Our fallback alternative for handler_t is a function pointer:
 */
typedef void (*handler_t)(socket_error_t, event_t *);

// TODO: The type of the buffers is TBD
// buffer_t is always passed by value to avoid losing temporary buffer_t's
typedef struct {
    void *p;
    size_t l;
} buffer_t;

class Socket {
protected:
    Socket(handler_t &defaultHandler);

protected:
    handler_t defaultHandler;
};

/* UDP socket class */
class UDPSocket: public Socket {
public:
    /* Socket Creation API */
    /**
     * UDP socket constructor.
     * Requires a port because the underlying UDP socket is allocated and bound at creation time. If an error occurs
     * during construction, the defaultHandler will be called.
     * @param[in] port The local port to listen on
     * @param[in] defaultHandler The default handler to use if no transmissions have been initiated or on error
     */
    UDPSocket(address_t *address, uint16_t port, handler_t &defaultHandler);
public:
    /* Socket Communication API */
    /**
     * Sends a datagram to the specified address.
     * The sendHandler will be called when the send completes.
     * @param[in] address The destination address
     * @param[in] port The destination port
     * @param[in] buffer The buffer to send
     * @param[in] flags The flags to use for the send.  Supported flags are: MSG_EOR, MSG_OOB.
     * @param[in] sendHandler The handler to call when the send is complete.  May be the Null Handler.
     * @return Returns an error code or 0 if the handler was installed and the transfer was started successfully.
     */
    socket_error_t send_to(address_t &address, uint16_t port, buffer_t buffer, int flags, handler_t &sendHandler);
    /**
     * Receives on the UDP socket with no address filtering
     * @param[in] buffer The buffer to receive into
     * @param[in] flags The flags used for receive. Supported flags are: MSG_OOB
     * @param[in] recvHandler The handler to call when the receive is complete.  May NOT be the Null handler.
     * @return Returns an error code or 0 if the handler was installed and the transfer was started successfully.
     */
    socket_error_t recv(buffer_t buffer, int flags, handler_t &recvHandler);
    /**
     * Receives on the UDP socket with address filtering
     * Open questions:
     *  o Should multiple recv_from's be permitted at once?
     *  o Should a recv be permitted at the same time as a recv_from?
     * @param[in] buffer The buffer to receive into
     * @param[in] flags The flags used for receive. Supported flags are: MSG_OOB
     * @param[in] recvHandler The handler to call when the receive is complete.  May NOT be the Null handler.
     * @return Returns an error code or 0 if the handler was installed and the transfer was started successfully.
     */
    socket_error_t recv_from(address_t &address, buffer_t buffer, int flags, handler_t &recvFromHandler);
    /**
     * Start a receive and a send simultaneously. Useful for command-response protocols.
     * @param[in] address The destination address
     * @param[in] port The destination port
     * @param[in] txBuffer The buffer to send
     * @param[in] txFlags The flags to use for the send.  Supported flags are: MSG_EOR, MSG_OOB.
     * @param[in] rxBuffer The buffer to receive into
     * @param[in] rxFlags flags The flags used for receive. Supported flags are: MSG_OOB
     * @param[in] sendRecvHandler The handler to call when the send completes and when the receive completes.  May NOT
     *              be the Null handler
     * @return Returns an error code or 0 if the handlers were installed and the transfer was started successfully.
     */
    socket_error_t send_recv(
            address_t &address,
            uin16_t port,
            buffer_t txBuffer,
            int txFlags,
            buffer_t rxBuffer,
            int rxFlags,
            handler_t &sendRecvHandler);
};

/* TCP Socket class */
class TCPSocket {
public:
    /* Object Management */
    ~TCPSocket();
public:
    /* Socket Creation API */
    /**
     * Listen for connections
     * This function is for creating a listening socket.  The acceptHandler will be called
     * each time there is an incoming connection attempt.  The acceptHandler can then decide
     * whether and how to allocate and configure a socket for the connection.
     * @param[in] address The address to listen on
     * @param[in] port The port to listen on
     * @param[in] acceptHandler The handler to call when a connection request is received.
     * @return Returns an error code or 0 if the handler was installed successfully.
     */
    static socket_error_t listen(address_t &address, uint16_t port, handler_t &acceptHandler);
    /**
     * TCP socket constructor
     * The default handler is required.  If a construction error is encountered, defaultHandler will be called
     * @param[in] defaultHandler The default handler to use if no transmissions have been initiated or on error
     */
    TCPSocket(handler_t &defaultHandler);
public:
    /* Connection Management API */
    /**
     * Connect to a remote host
     * @param[in] address Remote address
     * @param[in] port Remote port
     * @param[in] connectHandler Handler to call when the connect succeeds
     * @return Returns 0 if the connectHandler was installed correctly or an error otherwise.
     */
    socket_error_t connect(address_t &address, uint16_t port, handler_t &connectHandler);
    /**
     * Close the socket.
     * @return The exit status of the socket
     */
    socket_error_t close();
public:
    /* Socket Communication API */
    /**
     * Send a message over the open connection
     * @param[in] port The destination port
     * @param[in] buffer The buffer to send
     * @param[in] flags The flags to use for the send.  Supported flags are: MSG_EOR, MSG_OOB.
     * @param[in] sendHandler The handler to call when the send is complete.  May be the Null Handler.
     * @return Returns an error code or 0 if the handler was installed and the transfer was started successfully.
     */
    socket_error_t send(buffer_t buffer, int flags, handler_t &sendHandler);
    /**
     * Receive a message over the open connection
     * @param[in] buffer The buffer to receive into
     * @param[in] flags The flags used for receive. Supported flags are: MSG_OOB
     * @param[in] recvHandler The handler to call when the receive is complete.  May NOT be the Null handler.
     * @return Returns an error code or 0 if the handler was installed and the transfer was started successfully.
     */
    socket_error_t recv(buffer_t buffer, int flags, handler_t &recvHandler);
    /**
     * Send a message, then receive a message over the open connection
     * @param[in] txBuffer The buffer to send
     * @param[in] txFlags The flags to use for the send.  Supported flags are: MSG_EOR, MSG_OOB.
     * @param[in] rxBuffer The buffer to receive into
     * @param[in] rxFlags flags The flags used for receive. Supported flags are: MSG_OOB
     * @param[in] sendRecvHandler The handler to call when the send completes and when the receive completes.  May NOT
     *              be the Null handler
     * @return Returns an error code or 0 if the handlers were installed and the transfer was started successfully.
     */
    socket_error_t send_recv(
            buffer_t txBuffer,
            int txFlags,
            vbuffer_t rxBuffer,
            int rxFlags,
            handler_t &sendRecvHandler);
    /**
     * Start a continuous receive
     * This API should not be used with recv or send_recv. The application is responsible for appropriate deallocation
     * of buffers allocated by the allocator callback.
     * @param[in] allocator The allocator to use for storing received data
     * @param[in] contRecvHandler The handler to call when data is received
     * @return Returns an error code or 0 if the handlers were installed and the transfer was started successfully.
     */
    socket_error_t cont_recv_start(allocator_t &allocator, handler_t &contRecvHandler);
    /**
     * Stop an ongoing continuous receive
     * @return TBD: The exit status of the receive. Probably 0.
     */
    socket_error_t cont_recv_stop();

protected:
    /* API helper functions */
    virtual int event_handler(event_t *event, )
};



