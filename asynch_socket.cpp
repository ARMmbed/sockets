/*
 * udp_socket.c
 *
 *  Created on: 28 Nov 2014
 *      Author: bremor01
 */

#include "socket_types_impl.h"

#include "socket_api.h"
#include "socket_buffer.h"
#include "lwip/sockets.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"


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
    case (ERR_IF):
    break;
    }
    return err;
}

//static uint8_t family_remap(socket_proto_family_t family) {
//    uint8_t lwip_family = 0;
//    switch (family) {
//    case SOCKET_DGRAM:
//        lwip_family = SOCK_DGRAM;
//        break;
//    case SOCKET_STREAM:
//        lwip_family = SOCK_STREAM;
//        break;
//    case SOCKET_RAW:
//        lwip_family = SOCK_RAW;
//        break;
//    }
//    return lwip_family;
//}

socket_error_t socket_init() {
    return SOCKET_ERROR_NONE;
}

socket_error_t socket_create(struct socket *sock, socket_proto_family_t family, socket_api_handler handler)
{
    if (sock == NULL)
        return SOCKET_ERROR_NULL_PTR;
    switch (family) {
    case SOCKET_DGRAM:
        sock->impl.pcb.udp = udp_new();
        if (sock->impl.pcb.udp == NULL)
            return SOCKET_ERROR_BAD_ALLOC;
        break;
    case SOCKET_STREAM:
    default:
        return SOCKET_ERROR_BAD_FAMILY;
    }
    sock->family = family;
    sock->handler = (void*)handler;
    sock->impl.recv_arg = NULL;
    sock->status = SOCKET_STATUS_IDLE;
    return SOCKET_ERROR_NONE;
}

socket_error_t socket_destroy(struct socket *sock)
{
    if (sock == NULL)
        return SOCKET_ERROR_NULL_PTR;
    switch (sock->family) {
    case SOCKET_DGRAM:
        udp_remove(sock->impl.pcb.udp);
        break;
    case SOCKET_STREAM:
    default:
        return SOCKET_ERROR_BAD_FAMILY;
    }
    return SOCKET_ERROR_NONE;
}

socket_error_t socket_connect(struct socket *sock, const struct socket_addr *address, const uint16_t port) {
    err_t err = ERR_OK;
    switch (sock->family){
    case SOCKET_DGRAM:
        err = udp_connect(sock->impl.pcb.udp, (ip_addr_t *)address, port); break;
    case SOCKET_STREAM:
//        err = tcp_connect(sock->impl.pcb.tcp, (ip_addr_t *)address, port, (socket_api_handler)sock->handler); break;
    default:
        return SOCKET_ERROR_BAD_FAMILY;
    }
    return error_remap(err);
}
socket_error_t socket_bind(struct socket *sock, const struct socket_addr *address, const uint16_t port) {
    err_t err = ERR_OK;
    switch (sock->family){
    case SOCKET_DGRAM:
        err = udp_bind(sock->impl.pcb.udp, (ip_addr_t *)address, port); break;
    case SOCKET_STREAM:
        err = tcp_bind(sock->impl.pcb.tcp, (ip_addr_t *)address, port); break;
    default:
        return SOCKET_ERROR_BAD_FAMILY;
    }
    return error_remap(err);
}

socket_error_t socket_start_send(struct socket *sock, void *arg, struct socket_buffer *buf, uint8_t autofree)
{
    err_t err = ERR_OK;
    switch (sock->family) {
    case SOCKET_DGRAM:
        err = udp_send(sock->impl.pcb.udp, (struct pbuf *)buf); break;
    case SOCKET_STREAM:
        // TODO: TCP Send
    default:
        return SOCKET_ERROR_BAD_FAMILY;
    }
    if(err == ERR_OK) {
        sock->status = (socket_status_t)(SOCKET_STATUS_TX_BUSY|(int)sock->status);
        // Note: it looks like lwip sends do not require the buffer to persist.
        socket_api_handler handler = (socket_api_handler)sock->handler;
        socket_event_t e;
        e.event = SOCKET_EVENT_TX_DONE;
        e.i.t.context = arg;
        e.i.t.free_buf = autofree;
        e.i.t.buf = buf;
        e.i.t.sock = sock;
        sock->event = &e; // TODO: (CThunk upgrade/Alpha2)
        handler();
        if (e.i.t.free_buf) {
            socket_buf_try_free(buf);
        }
    }
    return error_remap(err);
}

static void recv_free(void *arg, struct udp_pcb *pcb, struct pbuf *p,
        ip_addr_t *addr, u16_t port)
{
    struct socket *s = (struct socket *)arg;
    socket_api_handler handler = (socket_api_handler)s->handler;
    socket_event_t e;
    e.event = SOCKET_EVENT_RX_DONE;
    e.i.r.buf = (struct socket_buffer *)p;
    e.i.r.context = s->impl.recv_arg;
    e.i.r.sock = s;
    e.i.r.port = port;
    e.i.r.src = (struct socket_addr *)addr;
    // Assume that the library will free the buffer unless the client
    // overrides the free.
    e.i.r.free_buf = 1;

    // Make sure the busy flag is cleared in case the client wants to start another receive
    s->status = (socket_status_t)((int)s->status & ~SOCKET_STATUS_RX_BUSY);

    s->event = &e; // TODO: (CThunk upgrade/Alpha2)
    handler();

    if(e.i.r.free_buf)
        socket_buf_free((struct socket_buffer *)p);
    s->impl.recv_arg = NULL;
}

socket_error_t socket_start_recv(struct socket *sock, void * arg) {
    err_t err = ERR_OK;

    if (socket_rx_is_busy(sock)) return SOCKET_ERROR_BUSY;
    sock->impl.recv_arg = arg;
    switch (sock->family) {
    case SOCKET_DGRAM:
        sock->status = (socket_status_t)((int)sock->status | SOCKET_STATUS_RX_BUSY);
        udp_recv(sock->impl.pcb.udp, recv_free, (void *)sock); break;
    case SOCKET_STREAM:
        //TODO: TCP receive
    default:
        return SOCKET_ERROR_BAD_FAMILY;
    }
    if(err == ERR_OK)
        sock->status = (socket_status_t)((int)sock->status | SOCKET_STATUS_RX_BUSY);
    return error_remap(err);

}

uint8_t socket_is_connected(struct socket *sock) {
    switch (sock->family) {
    case SOCKET_DGRAM:
        if (sock->impl.pcb.udp->flags & UDP_FLAGS_CONNECTED)
            return 1;
        return 0;
    case SOCKET_STREAM:
        //TODO: TCP is connected
    default:
        break;
    }
    return 0;
}
uint8_t socket_is_bound(struct socket *sock) {
    switch (sock->family) {
    case SOCKET_DGRAM:
        if (sock->impl.pcb.udp->local_port != 0)
            return 1;
        return 0;
    case SOCKET_STREAM:
        //TODO: TCP is bound
    default:
        break;
    }
    return 0;
}

uint8_t socket_tx_is_busy(struct socket *sock) {
    return !!(sock->status & SOCKET_STATUS_TX_BUSY);
}
uint8_t socket_rx_is_busy(struct socket *sock) {
    return !!(sock->status & SOCKET_STATUS_RX_BUSY);
}

