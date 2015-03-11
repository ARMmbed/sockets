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

#include "SocketAddr.h"

class aSocket {
protected:
    aSocket(const socket_stack_t stack);
    virtual ~aSocket();

public:
    socket_event_t *getEvent(); // TODO: (CThunk upgrade/Alpha3)

    socket_error_t resolve(const char* address, handler_t onDNS);

    virtual socket_error_t open(const socket_address_family_t af, const socket_proto_family_t pf);

    virtual void setOnError(handler_t onError);

    virtual void setOnReadable(handler_t onReadable);
    virtual socket_error_t recv(void * buf, size_t *len);
    virtual socket_error_t recv_from(void * buf, size_t *len, SocketAddr *remote_addr, uint16_t *remote_port);

    virtual void setOnWritable(handler_t onWritable);
    virtual socket_error_t send(const void * buf, const size_t len);
    virtual socket_error_t send_to(const void * buf, const size_t len, const SocketAddr *remote_addr, uint16_t remote_port);

    virtual socket_error_t close();

    static long ntohl(long);
    static short ntohs(short);
    static long long ntohll(long long);

protected:
    virtual void _eventHandler(struct socket_event *ev);
    bool error_check(socket_error_t err);

protected:
    // TODO: Should the DNS handler be static?
    handler_t _onDNS;
    handler_t _onError;
    handler_t _onReadable;
    handler_t _onWritable;

    CThunk<aSocket> _irq;
    struct socket _socket;
private:
    socket_event_t *_event; // TODO: (CThunk upgrade/Alpha3)
    void _nvEventHandler(void * arg);
protected:
    const socket_allocator_t *_alloc;
};


#endif // MBED_ASOCKET_H
