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
#ifndef __SOCKETS_V0_UDPSOCKET_H__
#define __SOCKETS_V0_UDPSOCKET_H__

#include <stddef.h>
#include <stdint.h>
#include "Socket.h"

namespace mbed {
namespace Sockets {
namespace v0 {

/* UDP socket class */
class UDPSocket: public Socket {
public:
    /**
     * UDP socket constructor.
     * Does not allocate an underlying UDP Socket instance.
     * @param[in] stack The network stack to use for this socket.
     */
    UDPSocket(socket_stack_t stack);
    /**
     * UDP Socket destructor
     */
    ~UDPSocket();
    /**
     * Open a UDP socket
     * Instantiates and initializes the underlying socket. Receive is started immediately after
     * the socket is opened.
     * @param[in] af Address family (SOCKET_AF_INET4 or SOCKET_AF_INET6), currently only IPv4 is supported
     * @retval SOCKET_ERROR_NONE on success
     * @retval SOCKET_ERROR_BAD_FAMILY if an invalid Address is supplied
     * @return Error code on failure
     */
    socket_error_t inline open(const socket_address_family_t af)
    {
        return Socket::open(af,SOCKET_DGRAM);
    }
    /**
     * Connect to a remote host.
     * This is an internal configuration API only.  No network traffic is generated.
     * @param[in] address The remote host to connect to
     * @param[in] port The remote port to connect to
     * @retval SOCKET_ERROR_NONE on success
     * @retval SOCKET_ERROR_NULL_PTR if address is NULL
     * @return Error code on failure
     */
    socket_error_t connect(const SocketAddr *address, const uint16_t port);
private:
    socket_error_t open(const socket_address_family_t af, const socket_proto_family_t pf);

};
} // namespace v0
} // namespace Sockets
} // namespace mbed
#endif // __SOCKETS_V0_UDPSOCKET_H__
