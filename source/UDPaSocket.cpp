#include "socket_types.h"
#include "UDPaSocket.h"

#include "socket_api.h"
#include "socket_buffer.h"
#include "SocketBuffer.h"

UDPaSocket::UDPaSocket(handler_t defaultHandler, const socket_allocator_t *alloc):
/* Store the default handler */
    aSocket(defaultHandler),
/* Zero the handlers */
    _recvHandler(NULL),
/* Zero the buffer pointers */
    _txBuf(NULL)
{
    _alloc = alloc;
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
UDPaSocket::bind(SocketAddr *address, uint16_t port)
{
    socket_error_t err = socket_bind(&_socket, address->getAddr(), port);
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

// socket_error_t
// UDPaSocket::start_send_to(const struct socket_addr *address, const uint16_t port, buffer_t *buffer, uint32_t flags, handler_t &sendHandler)
// {
//     if (socket_tx_is_busy(&_socket))
//         return SOCKET_ERROR_BUSY;
//     // TODO: need better support for queuing datagrams
//     if (_send_buffer) {
//         socket_error_t err = socket_buf_try_free(_send_buffer);
//         if (err != SOCKET_ERROR_NONE) return err;
//         _send_buffer = NULL;
//     }
//     _send_buffer = socket_buf_alloc(buffer->length, SOCKET_ALLOC_POOL_BEST);
//     if (_send_buffer == NULL) return SOCKET_ERROR_BAD_ALLOC;
//
//     socket_error_t err = socket_connect(&_socket, address, port);
//     if (err) return err;
//
//     _sendHandler = sendHandler;
//     err = socket_start_send_buf(&_socket, _send_buffer, flags);
//     return err;
// }

socket_error_t
UDPaSocket::start_send_to(SocketAddr *addr, const uint16_t port, const void *buffer, const size_t length, const int flags, handler_t &sendHandler)
{
  // Allocate a SocketBuffer
  SocketBuffer *sb = getBuffer(length);
  if(sb == NULL) {
    return SOCKET_ERROR_BAD_ALLOC;
  }
  sb->copyIn(buffer, length);
  sb->setTransferInfo(addr, port, sendHandler, flags);
  return start_send_to(sb);
}

socket_error_t
UDPaSocket::start_send_to(SocketAddr *addr, const uint16_t port, SocketBuffer *buffer, const int flags, handler_t &sendHandler)
{
    buffer->setTransferInfo(addr, port, sendHandler, flags);
    return start_send_to(buffer);
}
socket_error_t
UDPaSocket::start_send_to(SocketBuffer *sb) {
    // TODO: Make sure the correct buffer is used.
    if (_txBuf == NULL) {
        _txBuf = sb;
        socket_error_t err = socket_connect(&_socket, sb->getAddr()->getAddr(), sb->getPort());
        if (err) return err;
        err = socket_start_send(&_socket, sb->getCBuf(), &_socket);
        return err;
    } else {
        SocketBuffer *next = _txBuf;
        while (next->getNext() != NULL){
            next = next->getNext();
        }
        next->setNext(sb);
        return SOCKET_ERROR_NONE;
    }
}

socket_error_t
UDPaSocket::start_recv(handler_t &recvHandler)
{
    if( socket_rx_is_busy(&_socket)) {
        return SOCKET_ERROR_BUSY;
    }
    _recvHandler = recvHandler;
    socket_error_t err = socket_start_recv(&_socket);
    return err;
}

// TODO: SocketBuffer object must be passed to tx & rx handlers
// Workaround for TX is to extract TX buffer from socket
// Workaround for RX is to encapsulate pbuf in SocketBuffer in application event handler
void UDPaSocket::_eventHandler(void *arg) {
    (void) arg; // TODO: (CThunk upgrade/Alpha3)
    socket_event_t * e = getEvent(); // TODO: (CThunk upgrade/Alpha3)
    handler_t handler = _defaultHandler;
    SocketBuffer *buf = NULL;
    switch(e->event) {
    case SOCKET_EVENT_RX_DONE:
    case SOCKET_EVENT_RX_ERROR:
        handler = (_recvHandler?_recvHandler:_defaultHandler);
        break;
    case SOCKET_EVENT_TX_DONE:
    case SOCKET_EVENT_TX_ERROR:
      if (_txBuf->getHandler()) {
        handler = _txBuf->getHandler();
      }
      buf = _txBuf;
      if (buf->getNext()) {
        _txBuf = buf->getNext();
      }
      break;
    default:
        break;
    }
    handler(e);
    if (buf && buf->isFreeable()){
      delete buf;
    }
}
