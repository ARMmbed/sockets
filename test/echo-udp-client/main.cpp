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
#include "sal/socket_api.h"
#include <algorithm>
#include "sockets/UDPSocket.h"
#include "sal-iface-eth/EthernetInterface.h"
#include "sal-stack-lwip/lwipv4_init.h"
#include "minar/minar.h"
#include "core-util/FunctionPointer.h"
#include "greentea-client/test_env.h"
#include "utest/utest.h"
#include "unity/unity.h"

#define CHECK(RC, STEP)       if (RC < 0) error(STEP": %d\r\n", RC)

using namespace utest::v1;
using namespace mbed::Sockets::v0;


namespace {
    const int BUFFER_SIZE = 64;
    const int MAX_ECHO_LOOPS = 100;
    const char ASCII_MAX = '~' - ' ';
}

char char_rand() {
    return (rand() % ASCII_MAX) + ' ';
}

#ifndef min
#define min(A,B) \
    ((A)<(B)?(A):(B))
#endif

class UDPEchoClient;
typedef mbed::util::FunctionPointer2<void, bool, UDPEchoClient*> fpterminate_t;
void terminate(bool status, UDPEchoClient* client);

char buffer[BUFFER_SIZE] = {0};
int port = 0;
UDPEchoClient *client;
EthernetInterface eth;

class UDPEchoClient {
public:
    UDPEchoClient(socket_stack_t stack) :
        _usock(stack)
    {
        _usock.setOnError(UDPSocket::ErrorHandler_t(this, &UDPEchoClient::onError));
    }
    ~UDPEchoClient(){
        if (_usock.isConnected())
            _usock.close();
    }
    void start_test(char * host_addr, uint16_t port)
    {
        loop_ctr = 0;
        _port = port;
        
        socket_error_t err = _usock.open(SOCKET_AF_INET4);
        TEST_ASSERT_EQUAL_MESSAGE(SOCKET_ERROR_NONE, err, "MBED: UDPClient unable to open socket" NL);
        printf("MBED: Trying to resolve address %s" NL, host_addr);
        err = _usock.resolve(host_addr,UDPSocket::DNSHandler_t(this, &UDPEchoClient::onDNS));
        TEST_ASSERT_EQUAL_MESSAGE(SOCKET_ERROR_NONE, err, "MBED: UDPClient failed to resolve host server address" NL);
    }
    void onError(Socket *s, socket_error_t err) {
        (void) s;
        TEST_ASSERT_NOT_EQUAL(SOCKET_ERROR_NONE, err);
        printf("MBED: Socket Error: %s (%d)\r\n", socket_strerror(err), err);
        minar::Scheduler::postCallback(fpterminate_t(terminate).bind(false,this));
    }
    void onDNS(Socket *s, struct socket_addr sa, const char * domain)
    {
        (void) s;
        /* Extract the Socket event to read the resolved address */
        _resolvedAddr.setAddr(&sa);
        _resolvedAddr.fmtIPv6(out_buffer, sizeof(out_buffer));
        printf("MBED: Resolved %s to %s\r\n", domain, out_buffer);

        /* TODO: add support for getting AF from addr */
        socket_error_t err = _usock.open(SOCKET_AF_INET4);
        TEST_ASSERT_EQUAL_MESSAGE(SOCKET_ERROR_NONE, err, "MBED: UDPClient failed to open socket!" NL);
        
        /* Register the read handler */
        _usock.setOnReadable(UDPSocket::ReadableHandler_t(this, &UDPEchoClient::onRx));
        /* Send the query packet to the remote host */
        send_test();
    }
    void onRx(Socket *s)
    {
        (void) s;
        unsigned int n = sizeof(buffer);
        socket_error_t err = _usock.recv(buffer, &n);
        TEST_ASSERT_EQUAL_MESSAGE(SOCKET_ERROR_NONE, err, "MBED: UDPClient failed to recv data!" NL);

        int rc = memcmp(buffer, out_buffer, min(BUFFER_SIZE,n));
        TEST_ASSERT_EQUAL_MESSAGE(0, rc, "MBED: UDPClient round trip data validation error!" NL);

        loop_ctr++;
        if (loop_ctr < MAX_ECHO_LOOPS) {
            send_test();

        }
        if (loop_ctr >= MAX_ECHO_LOOPS) {
            _usock.send_to(buffer, strlen(buffer), &_resolvedAddr, _port);
            minar::Scheduler::postCallback(fpterminate_t(terminate).bind(true,this));
        }
    }

protected:
    void send_test() {
        std::generate(out_buffer, out_buffer + BUFFER_SIZE, char_rand);
        socket_error_t err = _usock.send_to(out_buffer, sizeof(BUFFER_SIZE), &_resolvedAddr, _port);
        TEST_ASSERT_EQUAL_MESSAGE(SOCKET_ERROR_NONE, err, "MBED: UDPClient failed to send data!" NL);
    }
protected:
    UDPSocket _usock;
    SocketAddr _resolvedAddr;
    uint16_t _port;
    char out_buffer[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];
    uint32_t loop_ctr;
    volatile bool done;
};

void terminate(bool status, UDPEchoClient* )
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



control_t test_echo_udp_client()
{
    socket_error_t err = lwipv4_socket_init();
    TEST_ASSERT_EQUAL_MESSAGE(SOCKET_ERROR_NONE, err, "Failed to init LWIPv4 socket!");
    
    printf("MBED: Initializing ethernet connection." NL);
    //Use DHCP
    TEST_ASSERT_EQUAL_MESSAGE(0, eth.init(), "Failed to init LWIPv4 socket!");
    eth.connect();

    printf("MBED: IP Address is %s" NL, eth.getIPAddress());
    greentea_send_kv("target_ip", eth.getIPAddress());
    
    memset(buffer, 0, sizeof(buffer));
    port = 0;
    
    printf("UDPClient waiting for server IP and port..." NL);
    char recv_key[] = "host_port";
    char port_value[] = "65325";
    
    greentea_send_kv("host_ip", " ");
    TEST_ASSERT_NOT_EQUAL_MESSAGE(0, greentea_parse_kv(recv_key, buffer, sizeof(recv_key), sizeof(buffer)), "MBED: Failed to recv/parse key value from host!");
    
    greentea_send_kv("host_port", " ");
    TEST_ASSERT_NOT_EQUAL_MESSAGE(0, greentea_parse_kv(recv_key, port_value, sizeof(recv_key), sizeof(port_value)), "MBED: Failed to recv/parse key value from host!");
    
    sscanf(port_value, "%d", &port);
    
    
    client = new UDPEchoClient(SOCKET_STACK_LWIP_IPV4);

    {
        mbed::util::FunctionPointer2<void, char *, uint16_t> fp(client, &UDPEchoClient::start_test);
        minar::Scheduler::postCallback(fp.bind(buffer, port));
    }
    return CaseTimeout(25000);
}


// Cases --------------------------------------------------------------------------------------------------------------
Case cases[] = {
    Case("Test Echo UDP Client", test_echo_udp_client),
};

status_t greentea_setup(const size_t number_of_cases)
{
    // Handshake with greentea
    // Host test timeout should be more than target utest timeout to let target cleanup the test and send test summary.
    GREENTEA_SETUP(30, "udpecho_client_auto"); 
    return greentea_test_setup_handler(number_of_cases);
}

void greentea_teardown(const size_t passed, const size_t failed, const failure_t failure)
{
    greentea_test_teardown_handler(passed, failed, failure);
    GREENTEA_TESTSUITE_RESULT(failed == 0);
}

Specification specification(greentea_setup, cases, greentea_teardown);

void app_start(int, char*[])
{
    Harness::run(specification);
}

