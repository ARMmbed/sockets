#include "socket_types_impl.h"
#include "socket_types.h"
#include "UDPaSocket.h"

#include "socket_api.h"
#include "socket_buffer.h"

UDPaSocket::UDPaSocket(handler_t defaultHandler):
/* Store the default handler */
    aSocket(defaultHandler),
/* Zero the handlers */
    _sendHandler(NULL),_recvHandler(NULL),
/* Zero the buffer pointers */
    _send_buffer(NULL)
{
    socket_error_t err = socket_init();
    err = socket_create(&_socket, SOCKET_DGRAM, (void(*)(void))_irq.entry()); // TODO: (CThunk upgrade/Alpha2)
    if (err != SOCKET_ERROR_NONE) {
        socket_event_t e;
        e.event = SOCKET_EVENT_ERROR;
        e.i.e = err;
        _defaultHandler(&e);
        return;
    }
}

socket_error_t
UDPaSocket::bind(struct socket_addr *address, uint16_t port)
{
    socket_error_t err = socket_bind(&_socket, address, port);
    return err;
}

UDPaSocket::~UDPaSocket()
{
    socket_error_t err = socket_destroy(&_socket);
    if (err != SOCKET_ERROR_NONE) {
        socket_event_t e;
        e.event = SOCKET_EVENT_ERROR;
        e.i.e = err;
        _defaultHandler(&e);
    }
}

socket_error_t
UDPaSocket::start_send_to(const struct socket_addr *address, const uint16_t port, buffer_t *buffer, int flags, handler_t &sendHandler)
{
    if (socket_tx_is_busy(&_socket))
        return SOCKET_ERROR_BUSY;
    // TODO: need better support for queuing datagrams
    if (_send_buffer) {
        socket_error_t err = socket_buf_try_free(_send_buffer);
        if (err != SOCKET_ERROR_NONE) return err;
        _send_buffer = NULL;
    }
    _send_buffer = socket_buf_alloc(buffer->length, SOCKET_ALLOC_POOL_BEST);
    if (_send_buffer == NULL) return SOCKET_ERROR_BAD_ALLOC;

    if (!socket_is_connected(&_socket)) {
        socket_error_t err = socket_connect(&_socket, address, port);
        if (err) return err;
    }

    _sendHandler = sendHandler;
    socket_error_t err = socket_start_send(&_socket, NULL, _send_buffer, 1);
    return err;
}

socket_error_t
UDPaSocket::start_recv(buffer_t *buffer, int flags, handler_t &recvHandler)
{
    if( socket_rx_is_busy(&_socket)) {
        return SOCKET_ERROR_BUSY;
    }
    _recvHandler = recvHandler;
    socket_error_t err = socket_start_recv(&_socket, NULL);
    return err;
}

socket_error_t
UDPaSocket::send_recv(
        struct socket_addr *address,
        uint16_t port,
        buffer_t *txBuffer,
        int txFlags,
        buffer_t *rxBuffer,
        int rxFlags,
        handler_t &sendRecvHandler)
{
    if (socket_rx_is_busy(&_socket) || socket_tx_is_busy(&_socket))
        return SOCKET_ERROR_BUSY;
    socket_error_t err = start_recv(rxBuffer, txFlags, sendRecvHandler);
    if (err)
        return err;
    err = start_send_to(address, port, txBuffer, rxFlags, sendRecvHandler);
    return err;
}

void UDPaSocket::_eventHandler(void *arg) {
    socket_event_t * e = getEvent(); // TODO: (CThunk upgrade/Alpha2)
    handler_t handler = _defaultHandler;
    switch(e->event) {
    case SOCKET_EVENT_RX_DONE:
    case SOCKET_EVENT_RX_ERROR:
        handler = (_recvHandler?_recvHandler:_defaultHandler);
        break;
    case SOCKET_EVENT_TX_DONE:
    case SOCKET_EVENT_TX_ERROR:
        handler = (_sendHandler?_sendHandler:_defaultHandler);
        break;
    default:
        break;
    }
    handler(arg);
}
