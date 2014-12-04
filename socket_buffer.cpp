/*
 * socket_buffer.c
 *
 *  Created on: 2 Dec 2014
 *      Author: bremor01
 */

#include "lwip/pbuf.h"
#include "socket_types_impl.h"

#include "socket_buffer.h"

static inline pbuf_type lwip_socket_pool_remap(socket_alloc_pool_t p)
{
    pbuf_type lp = PBUF_POOL;
    switch (p) {
    case SOCKET_ALLOC_HEAP:
        lp = PBUF_RAM;
        break;
    case SOCKET_ALLOC_POOL_BEST:
        lp = PBUF_POOL;
        break;
    }
    return lp;
}

void * socket_buf_get_ptr(const struct socket_buffer *b)
{
    return b->impl.payload;
}
size_t socket_buf_get_size(const struct socket_buffer *b)
{
    return b->impl.len;
}
struct socket_buffer * socket_buf_alloc(const size_t len, const socket_alloc_pool_t p)
{
    return (struct socket_buffer *) (void *) pbuf_alloc(PBUF_TRANSPORT, len, lwip_socket_pool_remap(p));
}
socket_error_t socket_buf_try_free(struct socket_buffer *b)
{
    if (b == NULL)
        return SOCKET_ERROR_NULL_PTR;
    if (b->impl.ref > 1) {
        return SOCKET_ERROR_BUSY;
    }
    socket_buf_free(b);
    return SOCKET_ERROR_NULL_PTR;
}

void socket_buf_free(struct socket_buffer *b)
{
    pbuf_free(&(b->impl));
}
socket_error_t socket_copy_from_user(struct socket_buffer *b, const void *u, const size_t len)
{
    err_t err = pbuf_take(&(b->impl), u, len);
    return socket_error_remap(err);
}
uint16_t socket_copy_to_user(void *u, struct socket_buffer *b, const size_t len)
{
    uint16_t rc = pbuf_copy_partial(&(b->impl), u, len, 0);
    return rc;
}
