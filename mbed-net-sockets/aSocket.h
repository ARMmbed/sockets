/*
 * PackageLicenseDeclared: Apache-2.0
 * Copyright 2015 ARM Holdings PLC
 */
#ifndef MBED_ASOCKET_H
#define MBED_ASOCKET_H


#include <mbed.h>
#include <stddef.h>
#include <stdint.h>
#include "socket_types.h"
#include "CThunk.h"

// Namespaced elements
#include "SocketBuffer.h"

#include "SocketAddr.h"

#ifdef __LWIP_SOCKETS_H__
#error lwip/sockets.h already included
#endif

namespace lwip {
    #include "lwip/netif.h"
    #include "lwip/sockets.h"
    #include "lwip/tcp.h"
    #include "lwip/udp.h"
};

class aSocket {
protected:

    aSocket(handler_t &defaultHandler, const socket_stack_t stack);
    virtual ~aSocket() {}

protected:
    virtual void _eventHandler(void *) = 0;
    bool error_check(socket_error_t err);

protected:
    handler_t _defaultHandler;
    handler_t _onDNS;
    CThunk<aSocket> _irq;
    SocketAddr _remoteAddr;
    SocketAddr _localAddr;
    union {
        struct lwip::tcp_pcb lwip_tcp;
        lwip::udp_pcb lwip_udp;
    } _impl;
    struct socket _socket;

public:
    virtual socket_error_t close();
    virtual void abort();
    socket_event_t *getEvent(); // TODO: (CThunk upgrade/Alpha3)

    socket_error_t resolve(const char* address, handler_t onDNS);


    void setAllocator(const socket_allocator_t *alloc) {
        _alloc = alloc;
    }

    virtual SocketBuffer * getBuffer(const size_t len) {
        if (_alloc == NULL || _socket.stack == SOCKET_STACK_UNINIT || _socket.stack > SOCKET_STACK_MAX) {
            return NULL;
        }
        return SocketBuffer::mk(len, socket_buf_stack_to_buf(_socket.stack), _alloc);
    }
    virtual SocketBuffer * getBuffer(const size_t len, const socket_buffer_type_t type)
    {
        if (_alloc == NULL) {
            return NULL;
        }
        return SocketBuffer::mk(len, type, _alloc);
    }
    virtual SocketBuffer * getBuffer(const size_t len, const socket_buffer_type_t type, const socket_allocator_t * alloc) {
        return SocketBuffer::mk(len, type, alloc);
    }
    virtual SocketBuffer * getBuffer(void *buf, const size_t len) {
        return SocketBuffer::mk(buf, len);
    }
    virtual SocketBuffer * getBuffer(const struct socket_buffer *sb) {
        return SocketBuffer::mk(sb);
    }

private:
    socket_event_t *_event; // TODO: (CThunk upgrade/Alpha3)
    void _nvEventHandler(void * arg);
protected:
    const socket_allocator_t *_alloc;
};


#endif // MBED_ASOCKET_H
