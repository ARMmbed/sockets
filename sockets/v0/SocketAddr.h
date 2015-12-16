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
#ifndef __SOCKETS_V0_SOCKETADDR_H__
#define __SOCKETS_V0_SOCKETADDR_H__

#include "sal/socket_types.h"

namespace mbed {
namespace Sockets {
namespace v0 {
/**
 * SocketAddr is a container class for storing IP addresses.
 * NOTE: When inet_ntop and inet_pton are adopted, additional member functions will be added.
 *
 * SocketAddr provides a common interface for setting and getting IP addresses
 *
 * When an IPv4 address is set, SocketAddr stores it in IPv4-mapped IPv6. It is assumed that any address in the
 * ::ffff:0000:0000 address range should be treated as an IPv4 address.
 */
class SocketAddr {
public:
    /**
     * Get a pointer to the internal struct socket_addr storage
     *
     * @return The address of the internal struct
     */
    struct socket_addr * getAddr() {return &_addr;}
    /**
     * Get a const pointer to the internal struct socket_addr storage
     *
     * @return The address of the internal struct
     */
    const struct socket_addr * getAddr() const {return &_addr;}
    /**
     * Copy the contents of an existing struct socket_addr into the internal storage
     *
     * @param[in] addr the original address to copy
     */
    void setAddr(const struct socket_addr *addr);
    /**
     * Copy the contents of an existing SocketAddr into the internal storage
     *
     * @param[in] addr the original address to copy
     */
    void setAddr(const SocketAddr *addr);
    /**
     * Return the size of the internal storage.
     *
     * @return the space consumed by a SocketAddr
     */
    size_t getAddrSize() const {return sizeof(_addr.ipv6be);}
    /**
     * Check if the internal address is an IPv4 address
     * This checks if the address falls in the `::ffff:0000:0000` range.
     *
     * @retval true  the IP address is IPv4
     * @retval false otherwise
     */
    bool is_v4();

    /**
     * Format the IP address in IPv4 dotted-quad Format
     *
     * @param[out] buf the buffer to fill
     * @param[in] size the size of the buffer to fill. Must be at least 16 chars long (incl. terminator).
     *
     * @retval -1 invalid pointer to buffer
     * @retval -2 buffer size too small
     * @retval -3 address is an IPv6 address, cannot format as IPv4.
     */
    int fmtIPv4(char *buf, size_t size);

    /// Convenience function for using statically allocated buffers.
    template< size_t N >
    int fmtIPv4(char (&buf)[N]) {
        return fmtIPv4(buf, N);
    }

    /**
     * Format the IP address in IPv6 colon separated format
     *
     * If the IP address is IPv6, colon-separated format is used.
     * If the IP address is IPv4, then the address is formatted as a 96-bit prefix in colon-separated format, followed
     * by a 32-bit dotted-quad.
     *
     * NOTE: IPv6 is not supported, pending inet_ntop integration.
     *
     * @param[out] buf the buffer to fill
     * @param[in] size  the size of the buffer to fill.
     *                  Must be at least 23 chars long (incl. terminator) for an IPv4 address in IPv6 format.
     *                  Must be at least 46 chars long for an IPv6 address.
     *
     * @retval 0 formatting was successful
     * @retval -1 invalid pointer to buffer
     * @retval -2 buffer size too small
     */
    int fmtIPv6(char *buf, size_t size);

    /// Convenience function for using statically allocated buffers.
    template< size_t N >
    int fmtIPv6(char (&buf)[N]) {
        return fmtIPv6(buf, N);
    }
protected:
    struct socket_addr _addr; /**< 128-bits of internal storage for the IP address */
};
} // namespace v0
} // namespace Sockets
} // namespace mbed
#endif // __SOCKETS_V0_SOCKETADDR_H__
