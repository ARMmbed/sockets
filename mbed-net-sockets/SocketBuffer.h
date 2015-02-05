/*
* PackageLicenseDeclared: Apache-2.0
* Copyright 2015 ARM Holdings PLC
*/
#ifndef MBED_SocketBuffer_H
#define MBED_SocketBuffer_H

#include <stddef.h>
#include <stdint.h>

namespace lwip {
    #include "lwip/pbuf.h"
}


#include "SocketAddr.h"
#include "socket_types.h"
#include "socket_buffer.h"

class SocketBuffer {
public:
    SocketBuffer(const bool freeable=false);
    SocketBuffer(void * buf, const size_t length, const bool freeable=false);
    SocketBuffer(const struct socket_buffer *pbuf, const bool freeable=false);
    SocketBuffer(const SocketBuffer &refBuf, const bool freeable=false);
    SocketBuffer(const size_t length, const socket_buffer_type_t type, const socket_allocator_t * alloc, const bool freeable = false);

    ~SocketBuffer();

public:
  size_t size();
  size_t copyIn(const void *buf, const size_t size);
  size_t copyOut(void *buf, const size_t size) const;

  uint32_t getu32();
  uint16_t getu16();
  uint8_t  getu8();
  int32_t  geti32();
  int16_t  geti16();
  int8_t   geti8();
  char     getch();

  size_t   getstr(char* str, const size_t size);

  struct socket_buffer * getCBuf();

  void * getImpl() const;
  void * getRaw() const;


  void set(void *buf, const size_t len);
  void set(const struct socket_buffer *buf);

  void setTransferInfo(SocketAddr *addr, const uint16_t port, const handler_t handler, const uint32_t flags);
  void setHandler(const handler_t handler);
  void setPort(const uint32_t port);
  void setAddr(SocketAddr *addr);
  void setFlags(const uint32_t flags);
  void setNext(SocketBuffer *nb);

  handler_t getHandler() const;
  uint16_t getPort() const {return _port; }
  SocketAddr * getAddr() const { return _addr; }
  uint32_t getFlags() const { return _sb.flags; }
  SocketBuffer * getNext() const;

  bool isFreeable() { return _freeable; }
  void setImplAlloc(const socket_allocator_t *alloc );

protected:
  struct socket_buffer _sb;

  union {
      struct lwip::pbuf lwip_pbuf;
      struct socket_rawbuf raw;
  } _impl;


  SocketBuffer *_next;

  SocketAddr *_addr;
  uint16_t _port;
  handler_t _handler;

  bool _freeable;
  const socket_allocator_t * _implAlloc;
public:
    static SocketBuffer * mk(void * buf, const size_t length)
    {
        SocketBuffer * a = new SocketBuffer(buf, length, true);
        return a;
    }
    static SocketBuffer * mk(const struct socket_buffer *pbuf) {
        SocketBuffer * a = new SocketBuffer(pbuf, true);
        return a;
    }
    static SocketBuffer * mk(const size_t length, const socket_buffer_type_t type, const socket_allocator_t * alloc) {
        SocketBuffer * a = new SocketBuffer(length, type, alloc, true);
        return a;
    }
};

#endif // MBED_SocketBuffer_H
