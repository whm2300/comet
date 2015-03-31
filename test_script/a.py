#!/usr/bin/env python
#-*- utf-8 -*-

import socket
import time

def test():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(('192.168.1.148', 8888))
    s.send("abcdefghijkasdfasdfs")
    return s

if __name__ == '__main__':
    s = []
    for i in range(100):
        s.append(test())

    time.sleep(10)
