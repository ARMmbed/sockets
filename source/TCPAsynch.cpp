/*
 * PackageLicenseDeclared: Apache-2.0
 * Copyright 2015 ARM Holdings PLC
 */

#include <Ticker.h>
#include <mbed-net-sockets/TCPAsynch.h>
#include <mbed-net-socket-abstract/socket_api.h>

using namespace mbed;

uintptr_t TCPAsynch::_TCPSockets = 0;
Ticker TCPAsynch::_ticker;
handler_t TCPAsynch::_tick_handler = NULL;

TCPAsynch::TCPAsynch(const socket_stack_t stack) :
		Socket(stack)
{
	// TODO: Dual stack: This block belongs to connect/bind
}
socket_error_t TCPAsynch::open(const socket_address_family_t af)
{
	socket_error_t err = Socket::open(af, SOCKET_STREAM);
	if (err != SOCKET_ERROR_NONE)
		return err;
	if (_TCPSockets == 0) {
		timestamp_t timeout = _socket.api->periodic_interval(&_socket);
		void (*f)() = _socket.api->periodic_task(&_socket);
		_ticker.attach_us(f, timeout);
	}
	_TCPSockets++;
	return err;
}

TCPAsynch::~TCPAsynch()
{
	_socket.api->destroy(&_socket);
	_TCPSockets--;
	if (!_TCPSockets) {
		_ticker.detach();
	}
}
