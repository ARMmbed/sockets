/*
* PackageLicenseDeclared: Apache-2.0
* Copyright 2015 ARM Holdings PLC
*/
#ifndef MBED_SOCKETADDR_H
#define MBED_SOCKETADDR_H

#include "socket_types.h"

#ifdef __LWIP_IP_ADDR_H__
#error LWIP ip_addr.h included before SocketAddr.h
#endif
namespace lwip {
    #include "ipv4/lwip/ip_addr.h"
};

typedef union {
    lwip::ip_addr_t lwip;
} socket_addr_impl_t;

class SocketAddr {
public:
    const struct socket_addr * getAddr() const {return &_addr;}
    void * getImpl() {return &_impl;}
    void setAddr(const struct socket_addr *addr);
    void setAddr(const SocketAddr *addr);
    size_t getAddrSize() const {return sizeof(_impl);}
protected:
    struct socket_addr _addr;
    socket_addr_impl_t _impl;
};

#endif // MBED_SOCKETADDR_H
