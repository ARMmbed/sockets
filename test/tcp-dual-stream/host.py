#!/usr/bin/python

import socket
from multiprocessing import Process

NSERVERS=4
BASEPORT=15000

servers=[]

def serve(port):
    host = ''
    backlog = 5
    size = 1024
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((host,port))
    s.listen(backlog)
    print('listening on %d'%port)
    while 1:
        client, address = s.accept()
        while 1:
            data = client.recv(size)
            print('Port %d, "%s"'%(port,data))
            if data == None:
                print('Client disconnected (%d)'%port)
                break;
            client.send(data)

def f(name):
    print 'hello', name

if __name__ == '__main__':
    for x in range(NSERVERS):
        port = BASEPORT + x
        p = Process(target=serve, args=(port,))
        p.start()
        servers.append(p)
    for s in servers:
        s.join()
