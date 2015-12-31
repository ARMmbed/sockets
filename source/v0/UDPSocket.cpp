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
#include "sockets/v0/UDPSocket.h"
#include "sal/socket_api.h"

using namespace mbed::Sockets::v0;
UDPSocket::UDPSocket(socket_stack_t stack):
/* Store the default handler */
    Socket(stack)
{
    _socket.family = SOCKET_DGRAM;
}

UDPSocket::~UDPSocket()
{
}

socket_error_t UDPSocket::connect(const SocketAddr *address, const uint16_t port)
{
    if (_socket.api == NULL) {
        return SOCKET_ERROR_BAD_STACK;
    }
    socket_error_t err = _socket.api->connect(&_socket, address->getAddr(), port);
    return err;
}

socket_error_t UDPSocket::open(const socket_address_family_t af, const socket_proto_family_t pf)
{
    (void)af;
    (void)pf;
    return SOCKET_ERROR_UNIMPLEMENTED;
}
