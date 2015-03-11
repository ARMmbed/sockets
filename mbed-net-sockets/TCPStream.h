#ifndef MBED_TCPStream_H
#define MBED_TCPStream_H
/*
 * PackageLicenseDeclared: Apache-2.0
 * Copyright 2015 ARM Holdings PLC
 */
#include <stddef.h>
#include <stdint.h>
#include <Ticker.h>
#include "socket_types.h"
#include "TCPAsynch.h"
#include "buffer.h"

class TCPStream: public TCPAsynch {
public:
    /**
     * TCP socket constructor.
     * Does not allocate an underlying TCP Socket instance.
	 * @param[in] stack The network stack to use for this socket.
     */
	TCPStream(const socket_stack_t stack);
	/**
	 * TCP socket destructor
	 */
	~TCPStream();
    /**
     * Connect to a remote host.
     * Initates the TCP Connection process.  onConnect is called when the connection
     * is acknowledged.
     * @param[in] address The remote host to connect to
     * @param[in] port The remote port to connect to
	 * @param[in] onConnect
     * @return SOCKET_ERROR_NONE on success, or an error code on failure
     */
	socket_error_t connect(const SocketAddr *address, const uint16_t port,
			const handler_t onConnect);
	/**
	 * Set a disconnect handler
	 * This handler only needs to be configured once onConnect has been called
	 * @param[in] h the handler to call when a connection is disconnected
	 */
	void onDisconnect(const handler_t h) { _onDisconnect = h; }

protected:
	/**
	 * Internal event handler
	 * @param[in] ev the event to handle
	 */
	void _eventHandler(struct socket_event *ev);

protected:
	handler_t _onConnect;
	handler_t _onDisconnect;
	uint16_t _port;
};

#endif // MBED_TCPStream_H
