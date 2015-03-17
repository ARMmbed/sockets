#ifndef MBED_TCPListener_H
#define MBED_TCPListener_H
/*
 * PackageLicenseDeclared: Apache-2.0
 * Copyright 2015 ARM Holdings PLC
 */
#include <stddef.h>
#include <stdint.h>
#include "TCPAsynch.h"
#include "TCPStream.h"

namespace mbed {

class TCPListener: public TCPAsynch {
public:
    /* Socket Creation API */
    TCPListener(const socket_stack_t stack);
    ~TCPListener();

    socket_error_t start_listening(handler_t listenHandler, uint32_t backlog = 0);
    socket_error_t stop_listening();

    TCPStream * accept(struct socket *new_socket);

protected:
    void _eventHandler(void*);
    handler_t _onIncomming;
};

}
#endif // MBED_TCPListener_H
