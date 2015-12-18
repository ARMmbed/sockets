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

#include "sockets/v0/TCPListener.h"
#include "sockets/v0/TCPStream.h"
#include "sal/socket_api.h"
#include "minar/minar.h"

using namespace mbed::Sockets::v0;


TCPListener::TCPListener(const socket_stack_t stack) :
    TCPAsynch(stack), _onIncoming(NULL)
{
}
TCPListener::~TCPListener()
{
    stop_listening();
}

socket_error_t TCPListener::start_listening(IncomingHandler_t listenHandler, uint32_t backlog)
{
    if (_socket.api == NULL) {
        return SOCKET_ERROR_BAD_STACK;
    }

    _onIncoming = listenHandler;

    socket_error_t err = _socket.api->start_listen(&_socket, backlog);
    return err;
}
socket_error_t TCPListener::stop_listening()
{
    if (_socket.api == NULL) {
        return SOCKET_ERROR_BAD_STACK;
    }
    socket_error_t err = _socket.api->stop_listen(&_socket);
    return err;
}
TCPStream * TCPListener::accept(void *new_impl)
{
    struct socket new_socket = _socket;
    new_socket.impl = new_impl;
    new_socket.stack = _socket.stack;
    new_socket.family = _socket.family;
    socket_error_t err;
    TCPStream * stream = new TCPStream(&_socket, &new_socket, err);
    if(err != SOCKET_ERROR_NONE) {
        delete stream;
        return NULL;
    }
    return stream;
}
void TCPListener::reject(void * impl)
{
    //TODO: Add support for reject
    struct socket s;
    s.impl = impl;
    s.stack = _socket.stack;
    s.api = _socket.api;
    s.family = _socket.family;
    s.api->close(&s);
}

void TCPListener::_eventHandler(struct socket_event *ev)
{
    switch(ev->event) {
    case SOCKET_EVENT_RX_ERROR:
    case SOCKET_EVENT_TX_ERROR:
    case SOCKET_EVENT_ERROR:
        if (_onError)
            minar::Scheduler::postCallback(_onError.bind(this, ev->i.e));
        break;
    case SOCKET_EVENT_RX_DONE:
    case SOCKET_EVENT_TX_DONE:
    case SOCKET_EVENT_CONNECT:
    case SOCKET_EVENT_DISCONNECT:
        if(_onError)
            minar::Scheduler::postCallback(_onError.bind(this, SOCKET_ERROR_UNIMPLEMENTED));
        break;
    case SOCKET_EVENT_DNS:
        if (_onDNS)
            minar::Scheduler::postCallback(_onDNS.bind(this, ev->i.d.addr, ev->i.d.domain));
        break;
    case SOCKET_EVENT_ACCEPT:
        if (_onIncoming)
            minar::Scheduler::postCallback(_onIncoming.bind(this, ev->i.a.newimpl));
            //TODO: write reject API
        break;
    case SOCKET_EVENT_NONE:
    default:
        break;
    }
}
