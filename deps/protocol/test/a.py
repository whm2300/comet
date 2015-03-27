#!/usr/bin/env python
# -*- coding:utf-8 -*-

import comet_pb2
import requests

def test_id():
    req = comet_pb2.ReqCheckTokenByID()
    req.id = 10000000001 
    req.token = 'e67870c72543d731675a1a5a2ba5418a'

    pack = comet_pb2.ReqPacket()
    pack.mask = 0
    pack.udi = 'ei=1&ai=1&bd=pada'
    pack.reqNo = 0
    pack.rsaKeyVer = '1'
    pack.clientId = 111
    pack.action.append('ReqCheckTokenByID')
    pack.params.append(req.SerializeToString())

    r = requests.post('http://uac3.api.stage.eaglenet.cn/', data = pack.SerializeToString())
    res = comet_pb2.RspPacket()
    print r.content
    res.ParseFromString(r.content)

    if res.rescode == 0:
        res_inner_pack = comet_pb2.RspCheckTokenByID()
        res_inner_pack.ParseFromString(res.params[0])
        print 'code:', res_inner_pack.rescode
        print 'msg:', res_inner_pack.resmsg
    else:
        print res.rescode
        print res.resmsg

def test_relation():
    inner_pack = comet_pb2.ReqCheckRelationByID()
    inner_pack.fromID = 10000000001
    inner_pack.toID = 20000016

    pack = comet_pb2.ReqPacket()
    pack.mask = 0
    pack.udi = 'ei=1&ai=1&bd=pada'
    pack.reqNo = 0
    pack.rsaKeyVer = '1'
    pack.clientId = 111
    pack.action.append('ReqCheckRelationByID')
    pack.params.append(inner_pack.SerializeToString())

    r = requests.post('http://uac3.api.stage.eaglenet.cn/', data = pack.SerializeToString())
    res = comet_pb2.RspPacket()
    res.ParseFromString(r.content)
    if res.rescode == 0:
        res_inner_pack = comet_pb2.RspCheckRelationByID()
        res_inner_pack.ParseFromString(res.params[0])
        print 'code:', res_inner_pack.rescode
        print 'msg:', res_inner_pack.resmsg
    else:
        print res.rescode
        print res.resmsg
    

if __name__ == '__main__':
    #test_id()
    for i in range(10000):
        test_relation()
