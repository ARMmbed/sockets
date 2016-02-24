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
#include "sockets/TCPStream.h"
#include "sal-stack-lwip/lwipv4_init.h"
#include "sal-iface-eth/EthernetInterface.h"
#include "minar/minar.h"
#include "core-util/FunctionPointer.h"
#include "greentea-client/test_env.h"
#include "utest/utest.h"
#include "unity/unity.h"

using namespace utest::v1;
using namespace mbed::Sockets::v0;

EthernetInterface eth;

class TCPEchoClient;
typedef mbed::util::FunctionPointer2<void, bool, TCPEchoClient*> fpterminate_t;
void terminate(bool status, TCPEchoClient* client);

char out_buffer[] = "Hello World\n";
char buffer[256];
char out_success[] = "{{success}}\n{{end}}\n";
TCPEchoClient *client;
int port;

class TCPEchoClient {
public:
    TCPEchoClient(socket_stack_t stack) :
        _stream(stack), _done(false), _disconnected(true)
        {
            _stream.setOnError(TCPStream::ErrorHandler_t(this, &TCPEchoClient::onError));
        }
    ~TCPEchoClient(){
        if (_stream.isConnected())
            _stream.close();
    }
    void onError(Socket *s, socket_error_t err) {
        (void) s;
        TEST_ASSERT_NOT_EQUAL_MESSAGE(SOCKET_ERROR_NONE, err, socket_strerror(err));
        printf("MBED: Socket Error: %s (%d)\r\n", socket_strerror(err), err);
        _done = true;
        minar::Scheduler::postCallback(fpterminate_t(terminate).bind(false,this));
    }
    void start_test(char * host_addr, uint16_t port)
    {
        printf("Trying to resolve address %s" NL, host_addr);
        _port = port;
        _done = false;
        _disconnected = true;
        socket_error_t err = _stream.open(SOCKET_AF_INET4);
        TEST_ASSERT_EQUAL_MESSAGE(SOCKET_ERROR_NONE, err, "MBED: Failed to open socket!");
        
        err = _stream.resolve(host_addr,TCPStream::DNSHandler_t(this, &TCPEchoClient::onDNS));
        TEST_ASSERT_EQUAL_MESSAGE(SOCKET_ERROR_NONE, err, "MBED: Failed to resolve host address!");
    }
    void onDNS(Socket *s, struct socket_addr sa, const char* domain)
    {
        (void) s;
        _resolvedAddr.setAddr(&sa);
        /* TODO: add support for getting AF from addr */
        /* Open the socket */
        _resolvedAddr.fmtIPv6(buffer, sizeof(buffer));
        printf("MBED: Resolved %s to %s\r\n", domain, buffer);
        /* Register the read handler */
        _stream.setOnReadable(TCPStream::ReadableHandler_t(this, &TCPEchoClient::onRx));
        _stream.setOnSent(TCPStream::SentHandler_t(this, &TCPEchoClient::onSent));
        _stream.setOnDisconnect(TCPStream::DisconnectHandler_t(this, &TCPEchoClient::onDisconnect));
        /* Send the query packet to the remote host */
        socket_error_t err = _stream.connect(_resolvedAddr, _port, TCPStream::ConnectHandler_t(this,&TCPEchoClient::onConnect));
        TEST_ASSERT_EQUAL_MESSAGE(SOCKET_ERROR_NONE, err, "MBED: Failed to connect host server!");
    }
    void onConnect(TCPStream *s)
    {
        (void) s;
        _disconnected = false;
        _unacked = sizeof(out_buffer) - 1;
        printf ("MBED: Sending (%d bytes) to host: %s" NL, _unacked, out_buffer);
        socket_error_t err = _stream.send(out_buffer, sizeof(out_buffer) - 1);
        TEST_ASSERT_EQUAL_MESSAGE(SOCKET_ERROR_NONE, err, "MBED: TCPClient failed to send data!");
    }
    void onRx(Socket* s)
    {
        (void) s;
        size_t n = sizeof(buffer)-1;
        socket_error_t err = _stream.recv(buffer, &n);
        TEST_ASSERT_EQUAL_MESSAGE(SOCKET_ERROR_NONE, err, "MBED: TCPClient failed to recv data!");
        
        buffer[n] = 0;
        printf ("MBED: Rx (%d bytes) from host: %s" NL, n, buffer);
        if (!_done && n > 0)
        {
            TEST_ASSERT_EQUAL_STRING_LEN_MESSAGE(out_buffer, buffer, n, "MBED: TCPClient round trip data validation failed!");
            
            _unacked += sizeof(out_success) - 1;
            printf ("MBED: Sending (%d bytes) to host: %s" NL, _unacked, out_success);
            err = _stream.send(out_success, sizeof(out_success) - 1);
            _done = true;
            TEST_ASSERT_EQUAL_MESSAGE(SOCKET_ERROR_NONE, err, "MBED: TCPClient failed to send data!");
        }
        if (!_done) {
            // Failed to validate rceived data. Terminating...
            minar::Scheduler::postCallback(fpterminate_t(terminate).bind(false,this));
        }
    }
    void onSent(Socket *s, uint16_t nbytes)
    {
        (void) s;
        _unacked -= nbytes;
        printf ("MBED: Sent %d bytes" NL, nbytes);
        if (_done && (_unacked == 0)) {
            minar::Scheduler::postCallback(fpterminate_t(terminate).bind(true,this));
        }
    }
    void onDisconnect(TCPStream *s)
    {
        (void) s;
        _disconnected = true;
    }
protected:
    TCPStream _stream;
    SocketAddr _resolvedAddr;
    uint16_t _port;
    volatile bool _done;
    volatile bool _disconnected;
    volatile size_t _unacked;
};

void terminate(bool status, TCPEchoClient* )
{
    if (client) {
        printf("MBED: Test finished!");
        delete client;
        client = NULL;
        eth.disconnect();
        TEST_ASSERT_TRUE_MESSAGE(status, "MBED: test failed!");
        Harness::validate_callback();
    }
}

control_t test_echo_tcp_client()
{
    socket_error_t err = lwipv4_socket_init();
    TEST_ASSERT_EQUAL(SOCKET_ERROR_NONE, err);
    
    TEST_ASSERT_EQUAL(0, eth.init()); //Use DHCP
    eth.connect();

    printf("TCPClient IP Address is %s" NL, eth.getIPAddress());
    greentea_send_kv("target_ip", eth.getIPAddress());
    
    memset(buffer, 0, sizeof(buffer));
    port = 0;
    
    printf("TCPClient waiting for server IP and port..." NL);
    char recv_key[] = "host_port";
    char port_value[] = "65325";
    
    greentea_send_kv("host_ip", " ");
    TEST_ASSERT_NOT_EQUAL_MESSAGE(0, greentea_parse_kv(recv_key, buffer, sizeof(recv_key), sizeof(buffer)), "MBED: Failed to recv/parse key value from host test!");
    
    greentea_send_kv("host_port", " ");
    TEST_ASSERT_NOT_EQUAL_MESSAGE(0, greentea_parse_kv(recv_key, port_value, sizeof(recv_key), sizeof(port_value)), "MBED: Failed to recv/parse key value from host test!");
    
    sscanf(port_value, "%d", &port);
    
    
    client = new TCPEchoClient(SOCKET_STACK_LWIP_IPV4);

    {
        mbed::util::FunctionPointer2<void, char *, uint16_t> fp(client, &TCPEchoClient::start_test);
        minar::Scheduler::postCallback(fp.bind(buffer, port));
    }
    
    return CaseTimeout(15000);
}

// Cases --------------------------------------------------------------------------------------------------------------
Case cases[] = {
    Case("Test Echo TCP Client", test_echo_tcp_client),
};

status_t greentea_setup(const size_t number_of_cases)
{
    // Handshake with greentea
    // Host test timeout should be more than target utest timeout to let target cleanup the test and send test summary.
    GREENTEA_SETUP(20, "tcpecho_client_auto"); 
    return greentea_test_setup_handler(number_of_cases);
}

Specification specification(greentea_setup, cases);

void app_start(int, char*[])
{
    Harness::run(specification);
}

