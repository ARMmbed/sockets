#ifndef MBED_SOCKET_H
#define MBED_SOCKET_H

#include <stddef.h>
#include <stdint.h>
#include "socket_types.h"

class Socket {
protected:
    Socket(handler_t &defaultHandler) :
            _defaultHandler(&defaultHandler)
    {
    }

protected:
    virtual socket_error_t _eventHandler(socket_error_t err, event_t *event);
protected:
    handler_t _defaultHandler;
};

#endif // MBED_SOCKET_H
