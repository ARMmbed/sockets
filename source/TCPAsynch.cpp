
#include <Ticker.h>
#include "TCPAsynch.h"
#include "socket_api.h"


uintptr_t TCPAsynch::_TCPSockets = 0;
Ticker TCPAsynch::_ticker;
handler_t TCPAsynch::_tick_handler = NULL;

TCPAsynch::TCPAsynch(handler_t defaultHandler, const socket_stack_t stack):
    aSocket(defaultHandler, stack)
{
  // TODO: Dual stack: This block belongs to connect/bind
  socket_error_t err = _socket.api->init();
  if (error_check(err)) {
    return;
  }
  err = _socket.api->create(&_socket, SOCKET_STREAM, (void(*)(void))_irq.entry()); // TODO: (CThunk upgrade/Alpha2)
  if (error_check(err)) {
    return;
  }
  if (_TCPSockets == 0) {
    timestamp_t timeout = _socket.api->periodic_interval(&_socket);
    void (*f)() = _socket.api->periodic_task(&_socket);
    _ticker.attach_us(f,timeout);
  }
  _TCPSockets++;
}
TCPAsynch::~TCPAsynch()
{
  _TCPSockets--;
  if (!_TCPSockets) {
      _ticker.detach();
  }
}

socket_error_t TCPAsynch::bind(const char * addr, const uint16_t port)
{
    struct socket_addr sa;
    sa.impl = _localAddr.getImpl();
    socket_error_t err = _socket.api->str2addr(&_socket, &sa, addr);
    _localAddr.setAddr(&sa);
    if (err != SOCKET_ERROR_NONE) {
        return err;
    }
    return bind(&_localAddr, port);
}
socket_error_t TCPAsynch::bind(const SocketAddr * addr, const uint16_t port)
{
    socket_error_t err = _socket.api->bind(&_socket, addr->getAddr(), port);
    if (err == SOCKET_ERROR_NONE && addr != &_localAddr) {
        _localAddr.setAddr(addr);
    }
    return err;
}
