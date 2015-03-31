#!/usr/bin/env python
#-*- utf-8 -*-

import socket
import time
import struct

import comet_pb2 

login = comet_pb2.C2SLogin()
login.id = 10000000001
login.token = 'a69361b1bfdcfa5b604fb62a7132b569'
login.user_agent = "abcdef"

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(('192.168.1.148', 8888))
#s.connect(('192.168.1.148', 49356))
#head = chr(0x15) + chr(0x0) + chr(0x0) + chr(0x0)

from_id = 123456
to_id = 0
head = struct.pack("!4B2q", 0x15, 0x0, 0x0, 0x0, from_id, to_id)
data = struct.pack("!HBH", 10, 1, len(login.SerializeToString())) + login.SerializeToString()

s.send(head + data)

n = 0
while True:
    n += 1
    d = s.recv(1024)
    print '---------------------------------------------------',n, len(d)
    print d

    if (len(d) > 4):
        version_head, nalu, ttl, res, from_id, to_id, property_id, format_type, len0 = struct.unpack("!4B2qHbH", d[0:25])
        if (nalu&0x20 == 1):
            print "hear msg"
            continue
        data = d[len(d)-len0:];
    
        print "version:0x%x" % (version_head&0xf0)
        print "head len:0x%x" % (version_head&0x0f)
        print "nalu:0x%x" % nalu
        print "ttl:0x%x" % ttl
        print "res:0x%x" % res
        print "from_id:%d" % from_id
        print "to_id:%d" % to_id
        print "property_id:%d" % property_id
        print "format:%d" % format_type
        print "len0:%d" % len0
    
        msg = comet_pb2.S2CLogin()
        if (property_id == 11):
            msg.ParseFromString(data)
            print "code:%d, msg:%s, ping_time:%d" % (msg.code, msg.msg, msg.ping_time)
        elif (property_id == 12):
            msg = comet_pb2.OnlineStatus()
            msg.ParseFromString(data)
            print msg.id
            for i in msg.status:
                print ord(i)
        else:
            print "other msg"

    elif (len(d) == 4):
        version_head, nalu, ttl, res = struct.unpack("!4B", d)
        print "version:0x%x" % (version_head&0xf0)
        print "head len:0x%x" % (version_head&0x0f)
        print "nalu:0x%x" % nalu
        print "ttl:0x%x" % ttl
        print "res:0x%x" % res

    while True:
        #time.sleep(1)

        song = comet_pb2.Song()
        song.song_id = "1111"
        song.song_name = "apologize"
        song.source = 3
        song.singers = "Michael"
        song.pic_url = "www.hoho.com"
        head = struct.pack("!4B2q", 0x15, 0x40, 0x0, 0x0, 10000000001, 20000016)
        data = struct.pack("!HBH", 22, 1, len(song.SerializeToString())) + song.SerializeToString()
        s.send(head + data)
        print "send ok"
        #time.sleep(100)
