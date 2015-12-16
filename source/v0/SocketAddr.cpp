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

bool SocketAddr::is_v4() {
    return socket_addr_is_ipv4(&_addr);
}

#define OCTET_SIZE 3
#define SEPARATOR_SIZE 1
#define IPv4_STRLEN (4 * (OCTET_SIZE) + 3 * SEPARATOR_SIZE)
#define IPv6_QUAD_SIZE 4
#define IPv64_PREFIX ("::ffff:")
#define IPv64_PREFIX_STRLEN (sizeof(IPv64_PREFIX) - 1)
#define IPv64_STRLEN (IPv64_PREFIX_STRLEN + IPv4_STRLEN)


// Returns 0 on success
int SocketAddr::fmtIPv4(char *buf, size_t size)
{
    if (buf == NULL) return -1;
    if (size <= IPv4_STRLEN) return -2;
    if (!socket_addr_is_ipv4(&_addr)) return -3;

    uint8_t *v4ip = reinterpret_cast<uint8_t *>(&_addr.ipv6be[3]);
    int rc = snprintf(buf, size, "%d.%d.%d.%d", v4ip[0], v4ip[1], v4ip[2], v4ip[3] );
    // rc must be non-negative and less than n, for the entire string to have been written
    return (0 <= rc && rc < int(size)) ? 0 : -2;
}
int SocketAddr::fmtIPv6(char *buf, size_t size)
{
    if (buf == NULL) return -1;
    if (socket_addr_is_ipv4(&_addr)) {
        if (size <= IPv64_STRLEN) return -2;
        // copy `::ffff:` prefix into buffer WITHOUT terminator
        strncpy(buf, IPv64_PREFIX, IPv64_PREFIX_STRLEN);
        // format IPv4 address after prefix
        return fmtIPv4(buf + IPv64_PREFIX_STRLEN, size - IPv64_PREFIX_STRLEN);
    } else {
        //TODO: requires inet_ntop
        return -3;
    }
}
