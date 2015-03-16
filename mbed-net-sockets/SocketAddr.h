/*
* PackageLicenseDeclared: Apache-2.0
* Copyright 2015 ARM Holdings PLC
*/
#ifndef __MBED_NET_SOCKETS_SOCKETADDR_H__
#define __MBED_NET_SOCKETS_SOCKETADDR_H__

#include <mbed-net-socket-abstract/socket_types.h>

namespace mbed {

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

}; // namespace mbed
#endif // __MBED_NET_SOCKETS_SOCKETADDR_H__
