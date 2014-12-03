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
        _defaultHandler(defaultHandler), _irq(this)
    {
        _irq.callback(&aSocket::_nvEventHandler);
    }
    virtual ~aSocket() {}

protected:
    virtual void _eventHandler(void *);
protected:
    handler_t _defaultHandler;
    CThunk<aSocket> _irq;

public:
    struct socket _socket;

private:
    void _nvEventHandler(void * arg) {
        _eventHandler(arg);
    }
};


#endif // MBED_SOCKET_H
