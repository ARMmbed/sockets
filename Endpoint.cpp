/* Copyright (C) 2012 mbed.org, MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "Socket/Endpoint.h"
#include <string.h>

Endpoint::Endpoint()  {
    reset_address();
}

Endpoint::~Endpoint() {}

void Endpoint::reset_address(void) {
    _ipAddress[0] = '\0';
    ip_addr_set_zero(&_address);
    _port  = 0;
}

int Endpoint::set_address(const char* host, uint16_t port) {
    reset_address();

    // TODO: add dns? This will be an async operation now ...
    // if (result != 4) {
    //     // Resolve address with DNS
    //     struct hostent *host_address = lwip_gethostbyname(host);
    //     if (host_address == NULL)
    //         return -1; //Could not resolve address
    //     p_address = (char*)host_address->h_addr_list[0];
    // }
    ipaddr_aton(host, &_address);
    _port = port;
    return 0;
}

int Endpoint::set_address(ip_addr_t* host, uint16_t port) {
    ip_addr_set(&_address, host);
    _port = port;
    return 0;
}

const char* Endpoint::get_address() {
    strcpy(_ipAddress, ipaddr_ntoa(&_address));
    return _ipAddress;
}

uint16_t Endpoint::get_port() {
    return _port;
}
