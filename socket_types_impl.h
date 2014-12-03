/*
 * socket_types_impl.h
 *
 *  Created on: 28 Nov 2014
 *      Author: bremor01
 */

#ifndef LIBRARIES_NET_LWIP_SOCKET_SOCKET_TYPES_IMPL_H_
#define LIBRARIES_NET_LWIP_SOCKET_SOCKET_TYPES_IMPL_H_

#include "lwip/sockets.h"
#include "lwip/pbuf.h"
#include "socket_types.h"

struct socket {
    union {
        struct tcp_pcb *tcp;
        struct udp_pcb *udp;
    } pcb;
    void *handler;
    void *recv_arg;
    socket_status_t status;
    uint8_t family;
};

struct socket_buffer {
    struct pbuf impl;
};


socket_error_t socket_error_remap(err_t lwip_err);



#endif /* LIBRARIES_NET_LWIP_SOCKET_SOCKET_TYPES_IMPL_H_ */
