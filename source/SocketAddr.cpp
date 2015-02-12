/*
* PackageLicenseDeclared: Apache-2.0
* Copyright 2015 ARM Holdings PLC
*/
#include "SocketAddr.h"
#include <string.h>

void SocketAddr::setAddr(const struct socket_addr *addr) {
    _addr.type = addr->type;
    if (&_impl != addr->impl)
        memcpy(&_impl, addr->impl, sizeof(socket_addr_impl_t));
    _addr.impl = &_impl;
}
void SocketAddr::setAddr(const SocketAddr *addr) {
    setAddr(addr->getAddr());
}
