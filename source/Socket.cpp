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
#include <mbed-net-sockets/Socket.h>

#include <mbed-net-socket-abstract/socket_api.h>
#include "cmsis.h"

Socket::Socket(const socket_stack_t stack) :
	_onDNS(NULL), _onError(NULL), _onReadable(NULL), _onSent(NULL),
	_irq(this), _event(NULL)
{
    _irq.callback(&Socket::_nvEventHandler);
    _socket.handler = NULL;
    _socket.impl = NULL;
    _socket.stack = stack;
    _socket.api = socket_get_api(stack);
    if (_socket.api == NULL) {
        error_check(SOCKET_ERROR_NULL_PTR);
    }
}
Socket::~Socket()
{
    if(_socket.api != NULL && _socket.api->destroy != NULL) {
        socket_error_t err = _socket.api->destroy(&_socket);
        error_check(err);
    }
}

socket_error_t Socket::open(const socket_address_family_t af, const socket_proto_family_t pf)
{
    if (_socket.api == NULL) {
        return SOCKET_ERROR_BAD_STACK;
    }
	return _socket.api->create(&_socket, af, pf, (socket_api_handler_t)_irq.entry());
}


bool Socket::error_check(socket_error_t err)
{
    struct socket_event e;
    if (err == SOCKET_ERROR_NONE) {
        return false;
    }
    // If there is an error, schedule an error event.
    e.event = SOCKET_EVENT_ERROR;
    e.i.e = err;
    _socket.event = &e; // TODO: (CThunk upgrade/Alpha3)
    _event = &e; // TODO: (CThunk upgrade/Alpha3)
    _eventHandler(&e);
    _event = NULL; // TODO: (CThunk upgrade/Alpha3)
    return true;
}

void Socket::_eventHandler(struct socket_event *ev)
{
	switch(ev->event) {
    case SOCKET_EVENT_RX_ERROR:
    case SOCKET_EVENT_TX_ERROR:
    case SOCKET_EVENT_ERROR:
    	if (_onError)
    		_onError(ev->i.e);
    	break;
    case SOCKET_EVENT_RX_DONE:
    	if(_onReadable)
    		_onReadable(SOCKET_ERROR_NONE);
    	break;
    case SOCKET_EVENT_TX_DONE:
    	if (_onSent)
    		_onSent(SOCKET_ERROR_NONE);
    	break;
    case SOCKET_EVENT_DNS:
    	if (_onDNS)
    		_onDNS(SOCKET_ERROR_NONE);
    	break;
    case SOCKET_EVENT_CONNECT:
    case SOCKET_EVENT_DISCONNECT:
    case SOCKET_EVENT_ACCEPT:
	case SOCKET_EVENT_NONE:
    default:
    	break;
	}
}

void Socket::setOnError(handler_t onError)
{
	__disable_irq();
	_onError = onError;
	__enable_irq();
}
void Socket::setOnReadable(handler_t onReadable)
{
	__disable_irq();
	_onReadable = onReadable;
	__enable_irq();
}
void Socket::setOnSent(handler_t onSent)
{
	__disable_irq();
	_onSent = onSent;
	__enable_irq();
}

void Socket::_nvEventHandler(void * arg)
{
    (void) arg;
    // Extract the event
    _event = _socket.event; // TODO: (CThunk upgrade/Alpha3)
    // Call the event handler
	_eventHandler(_event);
    _event = NULL; // TODO: (CThunk upgrade/Alpha3)
}

socket_event_t * Socket::getEvent()
{
	// Note that events are only valid while in the event handler
    return _event; // TODO: (CThunk upgrade/Alpha3)
}

socket_error_t Socket::resolve(const char* address, handler_t onDNS)
{
	if (_socket.handler == NULL) {
		return SOCKET_ERROR_CLOSED;
	}
    _onDNS = onDNS;
    socket_error_t err = _socket.api->resolve(&_socket, address);
    return err;
}

socket_error_t Socket::bind(const char * addr, const uint16_t port)
{
    SocketAddr tmp;
    socket_error_t err = _socket.api->str2addr(&_socket, tmp.getAddr(), addr);
    if (err != SOCKET_ERROR_NONE) {
        return err;
    }
    return bind(&tmp, port);
}
socket_error_t Socket::bind(const SocketAddr * addr, const uint16_t port)
{
    socket_error_t err = _socket.api->bind(&_socket, addr->getAddr(), port);
    return err;
}

socket_error_t Socket::close()
{
    return _socket.api->close(&_socket);
}

socket_error_t Socket::recv(void * buf, size_t *len)
{
	return _socket.api->recv(&_socket, buf, len);
}
socket_error_t Socket::recv_from(void * buf, size_t *len, SocketAddr *remote_addr, uint16_t *remote_port)
{
	struct socket_addr addr;
	socket_error_t err = _socket.api->recv_from(&_socket, buf, len, &addr, remote_port);
	remote_addr->setAddr(&addr);
	return err;
}

socket_error_t Socket::send(const void * buf, const size_t len)
{
	return _socket.api->send(&_socket, buf, len);
}
socket_error_t Socket::send_to(const void * buf, const size_t len, const SocketAddr *remote_addr, uint16_t remote_port)
{
	return _socket.api->send_to(&_socket, buf, len, remote_addr->getAddr(), remote_port);
}
