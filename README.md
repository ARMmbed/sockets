# mbed C++ Socket API
The mbed C++ Socket API provides an interface to sockets which will be familiar to users of BSD sockets, but there are several differences compared to the typical BSD socket implementations.  In typical desktop BSD socket applications, the program will block a thread, while waiting for each socket call to complete.  This is conceptually simple, but not well-suited to event-based embedded systems.  In the C++ Socket API, each socket has a set of callbacks which trigger when events complete, much like the BSD Sockets work in asynchronous mode; instead of having a single SIGIO handler however, the C++ Socket API provides an event handler for each type of event.

## Usage Notes
This version is an alpha and there are some known problems.
### Event handler context
Event handling currently occurs in the interrupt context.  In an upcoming release, there will be provision for event handling in the main loop.
### Error handling
Errors are forwarded to the event registered with setOnError().  If this event is not set, errors are ignored.  All applications should register an onError event handler.

## Getting Started
The best place to start is the mbed-example-network module.  There are simple examples there.  However, for a simple overview, there are several examples below.

### UDP Examples
Creating a UDP Socket
```C++
UDPSocket s(SOCKET_STACK_LWIP_IPV4);
```
Set an error handler
```C++
s.setOnError(onError);
```
Opening a UDP Socket
```C++
s.open(SOCKET_AF_INET4);
```
Send some data
```C++
s.send_to(data, dlen, addr, port);
```
Recieve some data
```C++
s.setOnReadable(onRecv);
```

### TCP Examples
Creating a TCP Socket
```C++
TCPStream s(SOCKET_STACK_LWIP_IPV4);
```
Opening a TCP Socket
```C++
s.open(SOCKET_AF_INET4);
```
Connect to a host
```C++
s.connect(addr, port, onConnect);
```
Set the disconnect handler
```C++
s.setOnDisconnect(onDisconnect);
```
Send some data
```C++
s.send(data, dlen);
```
Recieve some data
```C++
s.setOnReadable(onRecv);
```
Close the socket
```C++
s.close();
```

### DNS Example
This is a complete example of resolving an address with DNS
```C++
#include "sockets/v0/UDPSocket.h"
#include "sal-stack-lwip/lwipv4_init.h"
#include "sal-iface-eth/EthernetInterface.h"
using namespace mbed::Sockets::v0;
class Resolver {
private:
    UDPSocket _sock;
public:
    Resolver() : _sock(SOCKET_STACK_LWIP_IPV4) {
        _sock.open(SOCKET_AF_INET4);
    }
    void onDNS(Socket *s, struct socket_addr addr, const char *domain) {
        (void) s;
        SocketAddr sa;
        char buf[16];
        sa.setAddr(&addr);
        sa.fmtIPv4(buf,sizeof(buf));
        printf("Resolved %s to %s\r\n", domain, buf);
    }
    socket_error_t resolve(const char * address) {
        printf("Resolving %s...\r\n", address);
        return _sock.resolve(address,UDPSocket::DNSHandler_t(this, &Resolver::onDNS));
    }
};

EthernetInterface eth;
Resolver *r;
void app_start(int argc, char *argv[]) {
    (void) argc;
    (void) argv;
    static Serial pc(USBTX, USBRX);
    pc.baud(115200);
    printf("Connecting to network...\r\n");
    eth.init();
    eth.connect();
    printf("Connected\r\n");
    lwipv4_socket_init();
    r = new Resolver();
    r->resolve("mbed.org");
}

```
