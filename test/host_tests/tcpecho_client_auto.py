# Copyright 2015 ARM Limited, All rights reserved
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import sys
import socket
import logging
from threading import Thread
from sys import stdout
from SocketServer import BaseRequestHandler, TCPServer
from mbed_host_tests import BaseHostTest


class TCPEchoClient_Handler(BaseRequestHandler):
    def handle(self):
        """
        One handle per connection
        """
        print ("HOST: TCPEchoClient_Handler: Connection received...")
        while True:
            try:
                data = self.request.recv(1024)
                if not data: break
                print ('HOST: TCPEchoClient_Handler: \n%s\n' % data)

                # If client finishes, sit on recv and terminate
                # after client closes connection.
                if '{{end}}' in data: continue

                # echo data back to the client
                self.request.sendall(data)
            except Exception as e:
                print ('HOST: TCPEchoClient_Handler: %s' % str(e))
                break


class TCPEchoClientTest(BaseHostTest):

    __result = None
    COUNT_MAX = 10
    count = 0
    uuid_sent = []
    uuid_recv = []

    def __init__(self):
        """
        Initialise test parameters.

        :return:
        """
        super(BaseHostTest, self).__init__()
        self.SERVER_IP = None # Will be determined after knowing the target IP
        self.SERVER_PORT = 0  # Let TCPServer choose an arbitrary port
        self.server = None
        self.server_thread = None
        self.target_ip = None

    @staticmethod
    def find_interface_to_target_addr(target_ip):
        """
        Finds IP address of the interface through which it is connected to the target.

        :return:
        """
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect((target_ip, 0)) # Target IP, Any port
        ip = s.getsockname()[0]
        s.close()
        return ip

    def setup_tcp_server(self):
        """
        sets up a TCP server for target to connect and send test data.

        :return:
        """
        # !NOTE: There should mechanism to assert in the host test
        if self.SERVER_IP is None:
            self.log("setup_tcp_server() called before determining server IP!")
            self.notify_complete(False)

        # Returning none will suppress host test from printing success code
        self.server = TCPServer((self.SERVER_IP, self.SERVER_PORT), TCPEchoClient_Handler)
        ip, port = self.server.server_address
        self.SERVER_PORT = port
        self.server.allow_reuse_address = True
        self.log("HOST: Listening for TCP connections: " + self.SERVER_IP + ":" + str(self.SERVER_PORT))
        self.server_thread = Thread(target=TCPEchoClientTest.server_thread_func, args=(self,))
        self.server_thread.start()

    @staticmethod
    def server_thread_func(this):
        """
        Thread function to run TCP server forever.

        :param this:
        :return:
        """
        this.server.serve_forever()

    def _callback_target_ip(self, key, value, timestamp):
        """
        Callback to handle reception of target's IP address.

        :param key:
        :param value:
        :param timestamp:
        :return:
        """
        self.target_ip = value
        self.SERVER_IP = self.find_interface_to_target_addr(self.target_ip)
        self.setup_tcp_server()

    def _callback_host_ip(self, key, value, timestamp):
        """
        Callback for request for host IP Addr

        """
        self.send_kv("host_ip", self.SERVER_IP)

    def _callback_host_port(self, key, value, timestamp):
        """
        Callback for request for host port
        """
        self.send_kv("host_port", self.SERVER_PORT)

    def setup(self):
        self.register_callback("target_ip", self._callback_target_ip)
        self.register_callback("host_ip", self._callback_host_ip)
        self.register_callback("host_port", self._callback_host_port)

    def result(self):
        self.__result = self.uuid_sent == self.uuid_recv
        return self.__result

    def teardown(self):
        if self.server:
            self.server.shutdown()
            self.server_thread.join()
