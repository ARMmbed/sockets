/*
 * PackageLicenseDeclared: Apache-2.0
 * Copyright (c) 2015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <string.h>
#include <stdio.h>

#include "sockets/v0/SocketAddr.h"
#include "sal/socket_api.h"

using namespace mbed::Sockets::v0;

void SocketAddr::setAddr(const struct socket_addr *addr) {
    socket_addr_copy(&_addr, addr);
}
void SocketAddr::setAddr(const SocketAddr *addr) {
    setAddr(addr->getAddr());
}
socket_error_t SocketAddr::setAddr(socket_address_family_t af, const char *addr) {
    int rc = inet_pton(af, addr, &_addr);
    // Convert from inet_pton return codes to -1/0
    switch (rc) {
        case 1:
            return SOCKET_ERROR_NONE;
        case 0:
            return SOCKET_ERROR_BAD_ADDRESS;
        case -1:
            return SOCKET_ERROR_BAD_ARGUMENT;
        default:
            return SOCKET_ERROR_UNKNOWN;
    }
}

bool SocketAddr::is_v4() const {
    return socket_addr_is_ipv4(&_addr);
}

// Returns 0 on success
int SocketAddr::fmtIPv4(char *buf, size_t size) const
{
    if (!is_v4()){
        return -1;
    }
    if (buf == NULL) {
        return -1;
    }
    char * ptr = inet_ntop(SOCKET_AF_INET4, &(_addr.ipv6be[3]), buf, size);
    return (ptr == NULL)?-1:0;
}
int SocketAddr::fmtIPv6(char *buf, size_t size) const
{
    if (buf == NULL) {
        return -1;
    }
    char * ptr = inet_ntop(SOCKET_AF_INET6, &_addr, buf, size);
    return (ptr == NULL)?-1:0;
}
