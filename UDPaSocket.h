#ifndef MBED_UDPaSocket_H
#define MBED_UDPaSocket_H

#include <stddef.h>
#include <stdint.h>
#include "socket_types.h"
#include "Socket.h"

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
    UDPaSocket(address_t *address, uint16_t port, handler_t &defaultHandler);
    ~UDPaSocket();
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
    socket_error_t start_send_to(address_t *address, uint16_t port, buffer_t *buffer, int flags, handler_t &sendHandler);
    /**
     * Receives on the UDP socket with no address filtering
     * @param[in] buffer The buffer to receive into
     * @param[in] flags The flags used for receive. Supported flags are: MSG_OOB
     * @param[in] recvHandler The handler to call when the receive is complete.  May NOT be the Null handler.
     * @return Returns an error code or 0 if the handler was installed and the transfer was started successfully.
     */
    socket_error_t start_recv(buffer_t *buffer, int flags, handler_t &recvHandler);
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
            uint16_t port,
            buffer_t *txBuffer,
            int txFlags,
            buffer_t *rxBuffer,
            int rxFlags,
            handler_t &sendRecvHandler);
protected:
    socket_error_t _eventHandler(socket_error_t err, event_t *event);
protected:
    handler_t _sendHandler;
    handler_t _recvHandler;
};

#endif // MBED_UDPaSocket_H
