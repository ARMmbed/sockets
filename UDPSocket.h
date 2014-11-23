/* Copyright (C) 2012 mbed.org, MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef UDPSOCKET_H
#define UDPSOCKET_H

//#include "Socket/Socket.h"
#include "Socket/Endpoint.h"
#include "lwip/udp.h"
/**
UDP Socket
*/

typedef void (*recv_callback_t)(const uint8_t* p, uint16_t len, Endpoint& remote);

// TODO: this used to derive from Socket. That still needs to happen eventually.
class UDPSocket {

public:
    /** Instantiate an UDP Socket.
    */
    UDPSocket();

    /**
     * Free the UDP socket object.
     */
    virtual ~UDPSocket();

    /** Init the UDP Client Socket without binding it to any specific port
    \return 0 on success, -1 on failure.
    */
    int init(void);

    /** Bind a UDP Server Socket to a specific port
    \param port The port to listen for incoming connections on
    \return 0 on success, -1 on failure.
    */
    int bind(uint16_t port);

    /** Join the multicast group at the given address
    \param address  The address of the multicast group
    \return 0 on success, -1 on failure.
    */
    //int join_multicast_group(const char* address);

    /** Set the socket in broadcasting mode
    \return 0 on success, -1 on failure.
    */
    //int set_broadcasting(bool broadcast=true);

    /** Send a packet to a remote endpoint
    \param remote   The remote endpoint
    \param packet   The packet to be sent
    \param length   The length of the packet to be sent
    \return the number of written bytes on success (>=0) or -1 on failure
    */
    int sendTo(Endpoint &remote, char *packet, int length);

    /** Receive a packet from a remote endpoint
    \param remote   The remote endpoint
    \param buffer   The buffer for storing the incoming packet data. If a packet
           is too long to fit in the supplied buffer, excess bytes are discarded
    \param length   The length of the buffer
    \return the number of received bytes on success (>=0) or -1 on failure
    */
    // int receiveFrom(Endpoint &remote, char *buffer, int length);

    /**
     * Set the function to be called when data is received
     * @param _recv_cb - pointer to callback
     */
    void setReceiveCallback(recv_callback_t recv_cb) {
        _recv_cb = recv_cb;
    }

    /**
     * Returns the receive callback
     * @return Pointer to the receive callback
     */
    recv_callback_t getCallback() {
        return _recv_cb;
    }

    /** Close the socket
     */
    int close();

private:
    struct udp_pcb *_pcb;
    recv_callback_t _recv_cb;
};

#endif
