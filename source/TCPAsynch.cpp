
#include <Ticker.h>
#include "TCPAsynch.h"
#include "socket_api.h"


uintptr_t TCPAsynch::_TCPSockets = 0;
Ticker TCPAsynch::_ticker;
handler_t TCPAsynch::_tick_handler = NULL;

TCPAsynch::TCPAsynch(handler_t defaultHandler):
    aSocket(defaultHandler)
{
  // TODO: Dual stack: This block belongs to connect/bind
  socket_error_t err = socket_init();
  err = socket_create(&_socket, SOCKET_STREAM, (void(*)(void))_irq.entry()); // TODO: (CThunk upgrade/Alpha2)
  if (err != SOCKET_ERROR_NONE) {
    socket_event_t e;
    e.event = SOCKET_EVENT_ERROR;
    e.i.e = err;
    _defaultHandler(&e);
    return;
  }
  if (_TCPSockets == 0) {
    timestamp_t timeout = socket_periodic_interval(&_socket);
    void (*f)() = socket_periodic_task(&_socket);
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
