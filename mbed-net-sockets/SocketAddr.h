/*
* PackageLicenseDeclared: Apache-2.0
* Copyright 2015 ARM Holdings PLC
*/
#ifndef MBED_SOCKETADDR_H
#define MBED_SOCKETADDR_H

#ifdef __LWIP_IP_ADDR_H__
#error LWIP ip_addr.h included before SocketAddr.h
#endif

namespace lwip {
    #include "ipv4/lwip/ip_addr.h"
};

class SocketAddr {
public:
    void * getAddr() {return &_addr;}
    size_t getAddrSize() {return sizeof(_addr);}
protected:
    union {
        lwip::ip_addr_t lwip;
    } _addr;
};

#endif // MBED_SOCKETADDR_H
