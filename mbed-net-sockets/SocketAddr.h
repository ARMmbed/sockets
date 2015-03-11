/*
* PackageLicenseDeclared: Apache-2.0
* Copyright 2015 ARM Holdings PLC
*/
#ifndef MBED_SOCKETADDR_H
#define MBED_SOCKETADDR_H

#include "socket_types.h"

class SocketAddr {
public:
    struct socket_addr * getAddr() {return &_addr;}
    const struct socket_addr * getAddr() const {return &_addr;}
    void * getImpl() {return &_addr.storage;}
    void setAddr(const struct socket_addr *addr);
    void setAddr(const SocketAddr *addr);
    size_t getAddrSize() const {return sizeof(_addr.storage);}
protected:
    struct socket_addr _addr;
};

#endif // MBED_SOCKETADDR_H
