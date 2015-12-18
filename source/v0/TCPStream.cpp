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
#include "sockets/v0/TCPStream.h"
#include "sockets/v0/SocketAddr.h"
#include "sal/socket_api.h"
#include "minar/minar.h"

using namespace mbed::Sockets::v0;

TCPStream::TCPStream(const socket_stack_t stack) :
        /* Store the default handler */
        TCPAsynch(stack),
        /* Zero the handlers */
        _onConnect(NULL), _onDisconnect(NULL)
{
    /* NOTE: _socket is initialized by TCPAsynch. */
}
TCPStream::TCPStream(const struct socket *sock) :
        /* Store the default handler */
        TCPAsynch(sock->stack),
        /* Zero the handlers */
        _onConnect(NULL), _onDisconnect(NULL)
{
    _socket.family     = sock->family;
    _socket.impl       = sock->impl;
    socket_error_t err = _socket.api->accept(&_socket, reinterpret_cast<socket_api_handler_t>(_irq.entry()));
    error_check(err);
}
TCPStream::TCPStream(struct socket* listener, const struct socket *sock, socket_error_t &err) :
        /* Store the default handler */
        TCPAsynch(sock->stack),
        /* Zero the handlers */
        _onConnect(NULL), _onDisconnect(NULL)
{
    _socket.family     = sock->family;
    _socket.impl       = sock->impl;

    err = socket_accept(listener, &_socket,
        reinterpret_cast<socket_api_handler_t>(_irq.entry()));
}

TCPStream::~TCPStream()
{
}
socket_error_t TCPStream::connect(const SocketAddr &address, const uint16_t port, const ConnectHandler_t &onConnect)
{
    if (_socket.api == NULL){
        return SOCKET_ERROR_BAD_STACK;
    }
    _onConnect = onConnect;
    socket_error_t err = _socket.api->connect(&_socket, address.getAddr(), port);
    return err;
}

void TCPStream::_eventHandler(struct socket_event *ev)
{
    switch (ev->event) {
        case SOCKET_EVENT_CONNECT:
            if (_onConnect)
                minar::Scheduler::postCallback(_onConnect.bind(this));
            break;
        case SOCKET_EVENT_DISCONNECT:
            if (_onDisconnect)
                minar::Scheduler::postCallback(_onDisconnect.bind(this));
            break;
        default:
            // Call the aSocket event handler if the event is a generic one
            Socket::_eventHandler(ev);
            break;
    }
}

void TCPStream::setNagle(bool enable)
{
    void * enable_ptr;
    if(enable) {
        enable_ptr = (void*)1;
    } else {
        enable_ptr = NULL;
    }
    _socket.api->set_option(&_socket, SOCKET_PROTO_LEVEL_TCP, SOCKET_OPT_NAGLE, enable_ptr, 0);
}
