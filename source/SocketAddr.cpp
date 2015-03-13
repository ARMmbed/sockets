/*
 * PackageLicenseDeclared: Apache-2.0
 * Copyright 2015 ARM Holdings PLC
 */
#include <mbed-net-sockets/SocketAddr.h>
#include <string.h>

using namespace mbed;
void SocketAddr::setAddr(const struct socket_addr *addr) {
    _addr.type = addr->type;
    memcpy(_addr.storage, addr->storage, sizeof(_addr.storage));
}
void SocketAddr::setAddr(const SocketAddr *addr) {
    setAddr(addr->getAddr());
}
