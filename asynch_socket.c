/*
 * udp_socket.c
 *
 *  Created on: 28 Nov 2014
 *      Author: bremor01
 */

#include "socket_api.h"
#include "lwip/sockets.h"

#include "socket_types_impl.h"

socket_error_t error_remap(err_t lwip_err)
{
    socket_error_t err = SOCKET_ERROR_UNKNOWN;
    switch (lwip_err) {
    case ERR_OK:
        err = SOCKET_ERROR_NONE;
        break;
    case ERR_MEM:
        err = SOCKET_ERROR_BAD_ALLOC;
        break;
    case ERR_BUF:
        break;
    case ERR_TIMEOUT:
        err = SOCKET_ERROR_TIMEOUT;
        break;
    case ERR_RTE:
    case ERR_INPROGRESS:
    case ERR_WOULDBLOCK:
        err = SOCKET_ERROR_BUSY;
        break;
    case ERR_VAL:
    case ERR_USE:
    case ERR_ISCONN:
    case ERR_ABRT:
    case ERR_RST:
    case ERR_CLSD:
    case ERR_CONN:
    case ERR_ARG:
    case ERR_IF:
    }
    return err;
}

socket_error_t socket_init() {
    return SOCKET_ERROR_NONE;
}

socket_error_t socket_create(struct socket *sock, uint8_t family, socket_api_handler handler)
{
    if (sock == NULL)
        return SOCKET_ERROR_NULL_PTR;
    switch (family) {
    case SOCK_DGRAM:
        sock->pcb.udp = udp_new();
        if (sock->pcb.udp == NULL)
            return SOCKET_ERROR_BAD_ALLOC;
        break;
    case SOCK_STREAM:
    default:
        return SOCKET_ERROR_BAD_FAMILY;
    }
    sock->family = family;
    sock->handler = handler;
    sock->recv_arg = NULL;
    return SOCKET_ERROR_NONE;
}

socket_error_t socket_destroy(struct socket *sock)
{
    if (sock == NULL)
        return SOCKET_ERROR_NULL_PTR;
    switch (sock->family) {
    case SOCK_DGRAM:
        udp_remove(sock->pcb.udp);
        break;
    case SOCK_STREAM:
    default:
        return SOCKET_ERROR_BAD_FAMILY;
    }
    return SOCKET_ERROR_NONE;
}

socket_error_t socket_connect(struct socket *sock, address_t *address, uint16_t port) {
    err_t err = ERR_OK;
    switch (sock->family){
    case SOCK_DGRAM:
        err = udp_connect(sock->pcb.udp, address, port); break;
    case SOCK_STREAM:
        err = tcp_connect(sock->pcb.tcp,address, port, sock->handler); break;
    default:
        return SOCKET_ERROR_BAD_FAMILY;
    }
    return error_remap(err);
}
socket_error_t socket_bind(struct socket *sock, address_t *address, uint16_t port) {
    err_t err = ERR_OK;
    switch (sock->family){
    case SOCK_DGRAM:
        err = udp_bind(sock->pcb.udp, address, port); break;
    case SOCK_STREAM:
        err = tcp_bind(sock->pcb.tcp,address, port); break;
    default:
        return SOCKET_ERROR_BAD_FAMILY;
    }
    return error_remap(err);
}

socket_error_t socket_start_send(struct socket *sock, struct socket_buffer *buf) {
    err_t err = ERR_OK;
    switch (sock->family) {
    case SOCK_DGRAM:
        err = udp_send(sock->pcb.udp, buf); break;
    case SOCK_STREAM:
        // TODO: TCP Send
    default:
        return SOCKET_ERROR_BAD_FAMILY;
    }
    return error_remap(err);
}

static void recv_free(void *arg, struct udp_pcb *pcb, struct pbuf *p,
        ip_addr_t *addr, u16_t port)
{
    struct socket *s = (struct socket *)arg;
    struct socket_recv_info sri = { s->recv_arg, s, addr, port, (struct socket_buffer *)p, 1 };
    s->handler((void *)&sri);
    if(sri.free_buf)
        socket_buf_free((struct socket_buffer *)p);
    s->recv_arg = NULL;
}

socket_error_t socket_start_recv(struct socket *sock, void * arg) {
    err_t err = ERR_OK;
    if (sock->recv_arg != NULL) return SOCKET_ERROR_BUSY;
    sock->recv_arg = arg;
    switch (sock->family) {
    case SOCK_DGRAM:
        err = udp_recv(sock->pcb.udp, recv_free, (void *)sock); break;
    case SOCK_STREAM:
        //TODO: TCP receive
    default:
        return SOCKET_ERROR_BAD_FAMILY;
    }
    return error_remap(err);

}


