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
#include "lwip/ip_addr.h"

struct socket_impl {
    union {
        struct tcp_pcb *tcp;
        struct udp_pcb *udp;
    } pcb;
    void *recv_arg;
};

struct socket_buffer {
    struct pbuf impl;
};

struct socket_addr {
    ip_addr_t impl;
};

#include "socket_types.h"

socket_error_t socket_error_remap(err_t lwip_err);



#endif /* LIBRARIES_NET_LWIP_SOCKET_SOCKET_TYPES_IMPL_H_ */
