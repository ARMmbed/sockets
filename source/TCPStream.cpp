#include "socket_types.h"
#include "TCPStream.h"

#include "socket_api.h"
#include "socket_buffer.h"

TCPStream::TCPStream(const socket_stack_t stack) :
		/* Store the default handler */
		TCPAsynch(stack),
		/* Zero the handlers */
		_onConnect(NULL), _onDisconnect(NULL)
{
	//NOTE: _socket is initialized by TCPAsynch.
}

TCPStream::~TCPStream()
{
}
socket_error_t TCPStream::connect(const SocketAddr *address,
		const uint16_t port, const handler_t onConnect) {
	_onConnect = onConnect;
	socket_error_t err = _socket.api->connect(&_socket, address->getAddr(), port);
	return err;
}

void TCPStream::_eventHandler(struct socket_event *ev)
{
	switch (ev->event) {
	case SOCKET_EVENT_CONNECT:
		if (_onConnect)
			_onConnect(NULL);
		break;
	case SOCKET_EVENT_DISCONNECT:
		if (_onDisconnect)
			_onDisconnect(NULL);
		break;
	default:
		// Call the aSocket event handler if the event is a generic one
		aSocket::_eventHandler(ev);
		break;
	}
}
