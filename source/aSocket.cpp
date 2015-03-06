/*
 * PackageLicenseDeclared: Apache-2.0
 * Copyright 2015 ARM Holdings PLC
 */
#include "aSocket.h"
#include "socket_api.h"

aSocket::aSocket(handler_t &defaultHandler, const socket_stack_t stack) :
    _irq(this), _event(NULL),
	_onDNS(NULL), _onError(NULL), _onReadable(NULL), _onWritable(NULL)
{
    _irq.callback(&aSocket::_nvEventHandler);
    _socket.impl = NULL;
    _socket.stack = stack;
    _socket.api = socket_get_api(stack);
    if (_socket.api == NULL) {
        error_check(SOCKET_ERROR_NULL_PTR);
    }
}
aSocket::~aSocket()
{
    socket_error_t err = _socket.api->destroy(&_socket);
    error_check(err);
}

socket_error_t aSocket::open(const socket_address_family_t af, const socket_proto_family_t pf)
{
	return _socket.api->create(&_socket, af, pf, _irq.entry());
}


bool aSocket::error_check(socket_error_t err)
{
    struct socket_event e;
    if (err == SOCKET_ERROR_NONE) {
        return false;
    }
    e.event = SOCKET_EVENT_ERROR;
    e.i.e = err;
    _socket.event = &e; // TODO: (CThunk upgrade/Alpha3)
    _event = &e; // TODO: (CThunk upgrade/Alpha3)
    _eventHandler(&e);
    _event = NULL; // TODO: (CThunk upgrade/Alpha3)
    return true;
}

void aSocket::_eventHandler(struct socket_event *ev)
{
	switch(ev->event) {
    case SOCKET_EVENT_RX_ERROR:
    case SOCKET_EVENT_TX_ERROR:
    case SOCKET_EVENT_ERROR:
    	if (_onError)
    		_onError();
    	break;
    case SOCKET_EVENT_RX_DONE:
    	if(_onReadable)
    		_onReadable();
    	break;
    case SOCKET_EVENT_TX_DONE:
    	if (_onWritable)
    		_onWritable();
    	break;
    case SOCKET_EVENT_DNS:
    	if (_onDNS)
    		_onDNS();
    	break;
    case SOCKET_EVENT_CONNECT:
    case SOCKET_EVENT_DISCONNECT:
    case SOCKET_EVENT_ACCEPT:
	case SOCKET_EVENT_NONE:
    default:
    	break;
	}
}

void aSocket::setOnError(handler_t onError)
{
	_onError = onError;
}
void aSocket::setOnReadable(handler_t onReadable)
{
	_onReadable = onReadable;
}
void aSocket::setOnWritable(handler_t onWritable)
{
	_onWritable = onWritable;
}

void aSocket::_nvEventHandler(void * arg) {
    (void) arg;
    _event = _socket.event; // TODO: (CThunk upgrade/Alpha3)
	_eventHandler(_event);
    _event = NULL; // TODO: (CThunk upgrade/Alpha3)
}

socket_event_t * aSocket::getEvent()
{
    return _event; // TODO: (CThunk upgrade/Alpha3)
}

socket_error_t aSocket::resolve(const char* address, handler_t onDNS)
{
    _onDNS = onDNS;
    socket_error_t err = _socket.api->resolve(&_socket, address);
    return err;
}

socket_error_t aSocket::close()
{
    return _socket.api->close(&_socket);
}

size_t aSocket::recv(void * buf, size_t len)
{
	return _socket.api->recv(buf, len);
}
size_t aSocket::recv_from(void * buf, size_t len, SocketAddr *remote_addr, uint16_t *remote_port)
{
	struct socket_addr addr;
	size_t rlen = _socket.api->recv_from(buf, len, &addr, remote_port);
	remote_addr->setAddr(&addr);
	return rlen;
}

size_t aSocket::send(void * buf, size_t len)
{
	return _socket.api->send(buf, len);
}
size_t aSocket::send_to(void * buf, size_t len, SocketAddr *remote_addr, uint16_t remote_port)
{
	return _socket.api->send_to(buf, len, remote_addr->getAddr(), remote_port);
}
