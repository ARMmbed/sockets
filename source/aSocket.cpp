/*
 * PackageLicenseDeclared: Apache-2.0
 * Copyright 2015 ARM Holdings PLC
 */
#include "aSocket.h"
#include "socket_api.h"

aSocket::aSocket(handler_t &defaultHandler, const socket_stack_t stack) :
    _defaultHandler(defaultHandler), _irq(this), _event(NULL)
{
    _irq.callback(&aSocket::_nvEventHandler);
    _socket.impl = &_impl;
    _socket.stack = stack;
    _socket.api = socket_get_api(stack);

    if (_socket.api == NULL) {
        error_check(SOCKET_ERROR_NULL_PTR);
    }
}

bool aSocket::error_check(socket_error_t err)
{
    struct socket_event e;
    if (err == SOCKET_ERROR_NONE) {
        return false;
    }
    e.event = SOCKET_EVENT_ERROR;
    e.i.e = err;
    _socket.event = &e; // TODO: (CThunk upgrade/Alpha3)
    _event = &e; // TODO: (CThunk upgrade/Alpha3)
    _eventHandler(&e);
    _event = NULL; // TODO: (CThunk upgrade/Alpha3)
    return true;
}


void aSocket::_nvEventHandler(void * arg) {
    (void) arg;
    _event = _socket.event; // TODO: (CThunk upgrade/Alpha3)
    if (_event->event == SOCKET_EVENT_DNS) {
        _onDNS(_event);
    } else {
        _eventHandler(_event);
    }
    _event = NULL; // TODO: (CThunk upgrade/Alpha3)
}

socket_event_t * aSocket::getEvent()
{
    return _event; // TODO: (CThunk upgrade/Alpha3)
}

socket_error_t aSocket::resolve(const char* address, handler_t onDNS)
{
    _onDNS = onDNS;
    socket_error_t err = _socket.api->resolve(&_socket, address);
    return err;
}

socket_error_t aSocket::close()
{
    return _socket.api->close(&_socket);
}
void aSocket::abort()
{
    return _socket.api->abort(&_socket);
}
