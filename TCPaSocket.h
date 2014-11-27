#ifndef MBED_TCPaSocket_H
#define MBED_TCPaSocket_H

#include <stddef.h>
#include <stdint.h>
#include "socket_types.h"
#include "Socket.h"

/* TCP Socket class */
class TCPaSocket : public aSocket{
public:
    /* Object Management */
    ~TCPaSocket();
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
    TCPaSocket(handler_t &defaultHandler);
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


#endif // MBED_TCPaSocket_H

