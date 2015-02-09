#include "socket_types.h"
#include "TCPStream.h"

#include "socket_api.h"
#include "socket_buffer.h"

TCPStream::TCPStream(handler_t defaultHandler):
/* Store the default handler */
    TCPAsynch(defaultHandler),
/* Zero the handlers */
    _onSent(NULL), _onReceive(NULL), _onConnect(NULL),
/* Zero the buffer pointers */
    _txBuf(NULL)
{
    socket_error_t err = socket_init();
    err = socket_create(&_socket, SOCKET_STREAM, (void(*)(void))_irq.entry()); // TODO: (CThunk upgrade/Alpha3)
    if (err != SOCKET_ERROR_NONE) {
        socket_event_t e;
        e.event = SOCKET_EVENT_ERROR;
        e.i.e = err;
        // _defaultHandler(&e); //TODO: CThunk argument upgrade
        _socket.event = &e;
        _defaultHandler(&e);
    }
}

TCPStream::~TCPStream()
{
    // Handle any TX buffers
    SocketBuffer *sb = _txBuf;
    while(sb != NULL) {
        SocketBuffer *nsb = sb->getNext();
        if (sb->isFreeable()) {
            delete sb;
        }
        sb = nsb;
    }
    socket_destroy(&_socket);
}
socket_error_t
TCPStream::connect(SocketAddr *address, const uint16_t port, handler_t onConnect)
{
  _onConnect = onConnect;
  _port = port;
  socket_error_t err = socket_connect(&_socket, address, port);
  return err;
}

socket_error_t
TCPStream::start_send(void *buf, const size_t len, const handler_t &sendHandler, const uint32_t flags)
{
    if (!socket_is_connected(&_socket)) return SOCKET_ERROR_NO_CONNECTION;

    SocketBuffer *sb = getBuffer(buf, len);
    sb->setHandler(sendHandler);
    if (_txBuf == NULL) {
      _txBuf = sb;
      _txBuf->setFlags(flags);
      socket_error_t err = socket_start_send(&_socket, _txBuf->getCBuf(), &_socket);
      return err;
    }
    SocketBuffer * b = _txBuf;
    while (b->getNext() != NULL) { b = b->getNext();}
    b->setNext(sb);
    return SOCKET_ERROR_NONE;
}

socket_error_t
TCPStream::start_recv(handler_t &recvHandler)
{
    if( socket_rx_is_busy(&_socket)) {
        return SOCKET_ERROR_BUSY;
    }
    _onReceive = recvHandler;
    socket_error_t err = socket_start_recv(&_socket);
    return err;
}

// TODO: Need event object to hold event info.
// TBD: inheritance heirachy for events? structs/unions?
// TODO: Need to pass SocketBuffers to rx handlers
// Workaround: store the SocketBuffer locally and supply it to the caller via
// a getter method.
void TCPStream::_eventHandler(void *arg) {
    (void) arg;
    socket_event_t * e = getEvent(); // TODO: (CThunk upgrade/Alpha3)
    switch(e->event) {
    case SOCKET_EVENT_RX_DONE:
        _rxBuf.setImplFreeable(true);
        _rxBuf.set(&(e->i.r.buf));
        _onReceive(e);
        e->i.r.free_buf = _rxBuf.isImplFreeable();
        _rxBuf.set(NULL);
        break;
    case SOCKET_EVENT_TX_DONE:
        if (_txBuf && _txBuf->getHandler()) {
          _txBuf->getHandler()(e);
        } else {
          _defaultHandler(e);
        }
    break;
    case SOCKET_EVENT_CONNECT:
        _onConnect(e);
        break;
    case SOCKET_EVENT_DISCONNECT:
        _onDisconnect(e);
        break;
    default:
        _defaultHandler(e);
    break;
    }
}
