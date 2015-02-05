
#include <string.h>
#include <assert.h>
#include "SocketBuffer.h"
#include "socket_buffer.h"

SocketBuffer::SocketBuffer(const bool freeable) :
_next(NULL), _freeable(freeable), _implAlloc(NULL)
{
    _sb.impl = &_impl;
}
SocketBuffer::SocketBuffer(void * buf, const size_t length, const bool freeable):
_next(NULL), _freeable(freeable), _implAlloc(NULL)
{
    _sb.type = SOCKET_BUFFER_RAW;
    _sb.impl = &_impl;
    _impl.raw.buf = buf;
    _impl.raw.size = length;
    _impl.raw.pos = 0;
}
SocketBuffer::SocketBuffer(const struct socket_buffer *pbuf, const bool freeable):
_next(NULL), _freeable(freeable), _implAlloc(NULL)
{
    set(pbuf);
}
SocketBuffer::SocketBuffer(const SocketBuffer &refBuf, const bool freeable):
_next(NULL), _freeable(freeable), _implAlloc(NULL)
{
    set(&refBuf._sb);
    _next = refBuf._next;
    _addr = refBuf._addr;
    _sb.flags = refBuf._sb.flags;
    _port = refBuf._port;
    _handler = refBuf._handler;

    _freeable = refBuf._freeable;
}
SocketBuffer::SocketBuffer(const size_t length, const socket_buffer_type_t type, const socket_allocator_t * alloc, const bool freeable):
_next(NULL), _freeable(freeable), _implAlloc(alloc)
{
    _sb.impl = &_impl;
    _sb.type = type;
    if (type == SOCKET_BUFFER_RAW) {
        _impl.raw.buf = _implAlloc->alloc(_implAlloc->context, length);
        _impl.raw.size = length;
        // TODO: EMEM error check
    } else {
        socket_buf_alloc(length, SOCKET_ALLOC_POOL_BEST, &_sb);
    }
}

SocketBuffer::~SocketBuffer()
{
    if (_sb.type == SOCKET_BUFFER_RAW) {
        if (_implAlloc && _implAlloc->dealloc) {
            _implAlloc->dealloc(_implAlloc->context, _sb.impl);
        }
    } else {
        socket_buf_free(&_sb);
    }
}

size_t SocketBuffer::size()
{
    return socket_buf_get_size(&_sb);
}
size_t SocketBuffer::copyIn(const void *buf, const size_t size) {
    socket_error_t err = socket_copy_from_user(&_sb, buf, size);
    assert(err == SOCKET_ERROR_NONE);
    return size;
}
size_t SocketBuffer::copyOut(void *buf, const size_t size) const
{
    return socket_copy_to_user(buf, &_sb, size);
}

    // uint32_t getu32();
    // uint16_t getu16();
    // uint8_t  getu8();
    // int32_t  geti32();
    // int16_t  geti16();
    // int8_t   geti8();
    // char     getch();
    //
    // size_t   getstr(char* str, size_t size);

    // bool autodel();
    // void setAutodel(bool del);
    //
struct socket_buffer * SocketBuffer::getCBuf()
{
    return &_sb;
}
void * SocketBuffer::getImpl() const
{
    return (void *) &_impl;
}
void * SocketBuffer::getRaw() const
{
    if (_sb.type == SOCKET_BUFFER_RAW){
        return _impl.raw.buf;
    } else {
        return socket_buf_get_ptr(&_sb);
    }
}
    //
    // void set(void *buf, size_t len);
void SocketBuffer::set(const struct socket_buffer *buf)
{
    //TODO: Use stack's buffer copy to ensure reference counting.
    _sb.type = buf->type;
    _sb.impl = &_impl;
    memcpy(&_impl,buf->impl, sizeof(_impl));
}
    //
void SocketBuffer::setTransferInfo(SocketAddr *addr, const uint16_t port, const handler_t handler, const uint32_t flags)
{
    _addr = addr;
    _sb.flags = flags;
    _port = port;
    _handler = handler;
}

void SocketBuffer::setHandler(const handler_t handler)
{
    _handler = handler;
}
    // void setPort(const uint32_t port);
    // void setAddr(const SocketAddr *addr);
void SocketBuffer::setFlags(const uint32_t flags)
{
    _sb.flags = flags;
}
void SocketBuffer::setNext(SocketBuffer *nb)
{
    _next = nb;
}
    //
handler_t SocketBuffer::getHandler() const
{
    return _handler;
}
    // uint16_t getPort() const;
    // SocketAddr * getAddr() const;
    // uint32_t getFlags() const;
SocketBuffer * SocketBuffer::getNext()const
{
    return _next;
}
    // void setFreeable(bool freeable);
    // bool isFreeable();
