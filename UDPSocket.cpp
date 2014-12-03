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

#include "Socket/UDPSocket.h"
#include <cstring>
#include <cstdio>

using std::memcpy;

extern "C" void temp_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, ip_addr_t *addr, u16_t port) {
    UDPSocket *udps = reinterpret_cast<UDPSocket *>(arg);
    recv_callback_t cb = udps->getCallback();
    // TODO: is probably better to copy the data from the socket into a user-supplied buffer
    // (maybe specified as part of setRecvCallback)
    // TODO: the code below assumes that the pbuf is not chained; this might not be true for larger ammounts of data
    if (cb != NULL) {
        Endpoint remote(addr, port);
        cb((const uint8_t*)p->payload, p->len, remote);
    }
    pbuf_free(p);
}

UDPSocket::UDPSocket(): _pcb(NULL), _recv_cb(NULL) {
    _pcb = udp_new();
    udp_recv(_pcb, temp_recv, reinterpret_cast<void*>(this));
}

UDPSocket::~UDPSocket() {
    close();
}

int UDPSocket::init(void) {
    return 0;
}

// Server initialization
int UDPSocket::bind(uint16_t port) {
    udp_bind(_pcb, IP_ADDR_ANY, port);
    return 0;
}

int UDPSocket::close() {
    if (_pcb) {
        udp_remove(_pcb);
        _pcb = NULL;
    }
    return 0;
}

// int UDPSocket::join_multicast_group(const char* address) {
//     struct ip_mreq mreq;

//     // Set up group address
//     mreq.imr_multiaddr.s_addr = inet_addr(address);
//     mreq.imr_interface.s_addr = htonl(INADDR_ANY);

//     return set_option(IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
// }

// int UDPSocket::set_broadcasting(bool broadcast) {
//     int option = (broadcast) ? (1) : (0);
//     return set_option(SOL_SOCKET, SO_BROADCAST, &option, sizeof(option));
// }

// -1 if unsuccessful, else number of bytes written
int UDPSocket::sendTo(Endpoint &remote, char *packet, int length) {
    // TODO: take care of blocking/non-blocking
    // if (!_blocking) {
    //     TimeInterval timeout(_timeout);
    //     if (wait_writable(timeout) != 0)
    //         return 0;
    // }
    // This pbuf is automatically freed by the driver after being transmitted
    struct pbuf *p_out = pbuf_alloc(PBUF_TRANSPORT, length, PBUF_RAM);
    memcpy(p_out->payload, packet, length);
    udp_sendto(_pcb, p_out, &remote._address, remote._port);
    return length;
}
