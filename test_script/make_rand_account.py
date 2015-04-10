#!/usr/bin/env python
# -*- coding: utf-8 -*-

import random, string

import pymongo
import redis

redis_info = {'ip':'192.168.1.12', 'port':9979, 'db':20}
mongo_info = {'ip':'192.168.1.12', 'port':27017}
id_count = 2000000

def random_str(len):
    a = list(string.ascii_letters)
    random.shuffle(a)
    return ''.join(a[:len])

def make_id_token():
    id_token = []
    for i in range(1000000, 1000000+id_count):
        a = {}
        a['id'] = i
        a['token'] = random_str(32)
        a['relation'] = []
        for j in range(5):
            a['relation'].append(str(i/5*5 + j))
        a['relation'].remove(str(i))
        id_token.append(a)
    print 'make list over'
    save_to_redis(id_token)
    save_to_mongo(id_token)

def save_to_redis(id_token):
    r = redis.StrictRedis(host = redis_info['ip'], port = redis_info['port'], db = redis_info['db'])
    r.flushdb()
    for i in id_token:
        r.set('sToken:' + str(i['id']), i['token'])
        for j in i['relation']:
            r.sadd('sUD:' + str(i['id']), j)
    print 'save to redis over'

def save_to_mongo(id_token):
    for i in id_token:
        i.pop('relation')
    db = pymongo.MongoClient(mongo_info["ip"], mongo_info["port"])['comet']
    db['account_a'].drop()
    db['account_b'].drop()
    db['account_c'].drop()
    db['account_d'].drop()
    db['account_e'].drop()

    db['account_a'].insert(id_token[0:2000000:5])
    db['account_b'].insert(id_token[1:2000000:5])
    db['account_c'].insert(id_token[2:2000000:5])
    db['account_d'].insert(id_token[3:2000000:5])
    db['account_e'].insert(id_token[4:2000000:5])
    print 'save to mongo over'
            

def test_redis():
    r = redis.StrictRedis(host = redis_info['ip'], port = redis_info['port'], db = redis_info['db'])
    r.flushdb()
    r.set('asdf', 'aaaaaaa')
    r.sadd('21', 'asdfas')

if __name__ == '__main__':
    make_id_token()
