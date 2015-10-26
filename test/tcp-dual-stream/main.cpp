/*
 * PackageLicenseDeclared: Apache-2.0
 * Copyright (c) 2015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "mbed-drivers/mbed.h"
#include "mbed-drivers/test_env.h"
#include "sockets/TCPStream.h"
#include "sal/test/ctest_env.h"
#include "sal-stack-lwip/lwipv4_init.h"
#include "sal-iface-eth/EthernetInterface.h"
#include "minar/minar.h"
#include "core-util/FunctionPointer.h"

using namespace mbed::Sockets::v0;

EthernetInterface eth;

#define NSTREAMS 4
#define BASEPORT 15000
#define HOSTADDRESS "192.168.0.13"
#define BUFSIZE 64

struct conn_s{
    TCPStream *stream;
    uint16_t port;
    uint16_t packet;
    char buffer[BUFSIZE];
} connections[NSTREAMS] = {{0,0,0,0}};

static void send(uint index, TCPStream *stream)
{
    snprintf(connections[index].buffer,BUFSIZE,
        "01234567890123456789012345678901234567890123456789p%hus%u",connections[index].packet,index);
    socket_error_t err = stream->send(connections[index].buffer, BUFSIZE);
    if (!TEST_EQ(err, SOCKET_ERROR_NONE)) {
        printf("MBED: Expected %d, got %d (%s)\r\n", SOCKET_ERROR_NONE, err, socket_strerror(err));
    }
}

static void onRecv(Socket *s)
{
    TCPStream *stream = (TCPStream *)s;
    uint index = (uint)-1;
    /* Get the port */
    for (uint i = 0; i < NSTREAMS; i++) {
        if (connections[i].stream == stream) {
            index = i;
            break;
        }
    }
    size_t n = BUFSIZE;
    socket_error_t err = stream->recv(connections[index].buffer, &n);
    if (!TEST_EQ(err, SOCKET_ERROR_NONE)) {
        printf("MBED: Expected %d, got %d (%s)\r\n", SOCKET_ERROR_NONE, err, socket_strerror(err));
    }
    send(index, stream);
}

static void onConnect(TCPStream *stream) {
    uint index = (uint)-1;
    /* Get the port */
    for (uint i = 0; i < NSTREAMS; i++) {
        if (connections[i].stream == stream) {
            index = i;
            break;
        }
    }
    send(index, stream);
}

static void onDNS(Socket *s, struct socket_addr sa, const char* domain)
{
    char buffer[64];
    SocketAddr resolvedAddr;
    resolvedAddr.setAddr(&sa);
    /* Open the socket */
    resolvedAddr.fmtIPv4(buffer, sizeof(buffer));
    TCPStream *stream = (TCPStream *)s;
    printf("MBED: Resolved %s to %s\r\n", domain, buffer);
    socket_error_t err = stream->open(SOCKET_AF_INET4);
    TEST_EQ(err, SOCKET_ERROR_NONE);
    /* Register the read handler */
    stream->setOnReadable(TCPStream::ReadableHandler_t(onRecv));

    uint16_t port = 0;
    /* Get the port */
    for (uint i = 0; i < NSTREAMS; i++) {
        if (connections[i].stream == stream) {
            port = connections[i].port;
        }
    }
    if (port == 0) {
        printf("MBED: Stream %p is not valid\r\n", stream);
        return;
    }

    /* Send the query packet to the remote host */
    err = stream->connect(resolvedAddr, port, TCPStream::ConnectHandler_t(onConnect));
    if(!TEST_EQ(err, SOCKET_ERROR_NONE)) {
        printf("MBED: Expected %d, got %d (%s)\r\n", SOCKET_ERROR_NONE, err, socket_strerror(err));
    }
}

static void start_test(TCPStream *stream, const char *address)
{
    socket_error_t err = stream->resolve(address,TCPStream::DNSHandler_t(onDNS));
    if (!TEST_EQ(err, SOCKET_ERROR_NONE)) {
        printf("MBED: TCP Client unable to resolve %s" NL, address);
    }
}

void app_start(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    socket_error_t err = lwipv4_socket_init();
    TEST_EQ(err, SOCKET_ERROR_NONE);

    Serial pc(USBTX, USBRX);
    pc.baud(115200);

    eth.init(); //Use DHCP
    eth.connect();

    for (uint i = 0; i < NSTREAMS; i++) {
        TCPStream *client = new TCPStream(SOCKET_STACK_LWIP_IPV4);
        start_test( client, HOSTADDRESS);
        connections[i].stream = client;
        connections[i].port = BASEPORT+i;
        connections[i].packet = 0;
    }
}
