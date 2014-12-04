#ifndef MBED_SOCKET_H
#define MBED_SOCKET_H

#include <mbed.h>
#include <stddef.h>
#include <stdint.h>
#include "socket_types.h"
#include "socket_types_impl.h"
#include "CThunk.h"

class aSocket {
protected:

    aSocket(handler_t &defaultHandler) :
        _defaultHandler(defaultHandler), _irq(this), _event(NULL)
    {
        _irq.callback(&aSocket::_nvEventHandler);
    }
    virtual ~aSocket() {}

protected:
    virtual void _eventHandler(void *) = 0;
protected:
    handler_t _defaultHandler;
    CThunk<aSocket> _irq;
public:
    struct socket _socket;
    socket_event_t *getEvent(){return _event;} // TODO: (CThunk upgrade/Alpha2)

private:
    socket_event_t *_event; // TODO: (CThunk upgrade/Alpha2)
    void _nvEventHandler(void * arg) {
        _event = _socket.event; // TODO: (CThunk upgrade/Alpha2)
        _eventHandler(arg);
        _event = NULL; // TODO: (CThunk upgrade/Alpha2)
    }
};


#endif // MBED_SOCKET_H

