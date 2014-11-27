#ifndef MBED_SOCKET_H
#define MBED_SOCKET_H

#include <stddef.h>
#include <stdint.h>
#include "socket_types.h"
#include "CThunk.h"

class aSocket {
protected:

    aSocket(handler_t &defaultHandler) :
            _defaultHandler(&defaultHandler)
    {
        _irq.callback(&aSocket::_eventHandler);
    }

protected:
    virtual void _eventHandler(void *);
protected:
    handler_t _defaultHandler;
    CThunk<aSocket> _irq;
    socket_t _socket;
};


#endif // MBED_SOCKET_H
