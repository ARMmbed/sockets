/*
* PackageLicenseDeclared: Apache-2.0
* Copyright 2015 ARM Holdings PLC
*/
#include "SocketAddr.h"
#include <string.h>

void SocketAddr::setAddr(struct socket_addr *addr) {
    _addr.type = addr->type;
    memcpy(&_impl, addr->impl, sizeof(socket_addr_impl_t));
    _addr.impl = &_impl;
}
