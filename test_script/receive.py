#!/usr/bin/env python
#-*- utf-8 -*-

import socket
import time
import struct

import comet_pb2 

login = comet_pb2.C2SLogin()
login.id = 1000001
login.token = 'fCqciWmRYIHFVhQzpasoAJbKDrtENBkd'
login.user_agent = "abcdef"

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(('192.168.1.148', 8888))
#s.connect(('221.228.197.71', 49362))
#s.connect(('192.168.1.148', 49356))
#head = chr(0x15) + chr(0x0) + chr(0x0) + chr(0x0)

from_id = 12345
to_id = 0
head = struct.pack("!4B2q", 0x15, 0x0, 0x0, 0x0, from_id, to_id)
data = struct.pack("!HBH", 10, 1, len(login.SerializeToString())) + login.SerializeToString()

s.send(head + data)

n = 0
while True:
    n += 1
    d = s.recv(200)
    print '---------------------------------------------------',n, len(d)
    #print d

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

        if (property_id == 11):
            msg = comet_pb2.S2CLogin()
            msg.ParseFromString(data)
            print "code:%d, msg:%s, ping_time:%d" % (msg.code, msg.msg, msg.ping_time)
        elif (property_id == 22):
            msg = comet_pb2.Song()
            msg.ParseFromString(data)
            print "~song_id:%s, song_name:%s, source:%d, singers:%s, pic_url:%s" % (msg.song_id, msg.song_name, msg.source, msg.singers, msg.pic_url)
        elif (property_id == 12):
            msg = comet_pb2.OnlineStatus()
            msg.ParseFromString(data)
            print "online status:", msg.id
            for i in msg.status:
                print ord(i)
    elif (len(d) == 4):
        version_head, nalu, ttl, res = struct.unpack("!4B", d)
        print "version:0x%x" % (version_head&0xf0)
        print "head len:0x%x" % (version_head&0x0f)
        print "nalu:0x%x" % nalu
        print "ttl:0x%x" % ttl
        print "res:0x%x" % res


    #time.sleep(1)
    #s.send(struct.pack("!4B", 0x11, 0x20, 0x0, 0x0))
