#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <hiredis.h>
#include <async.h>
#include <libevent.h>

void getCallback(redisAsyncContext *c, void *r, void *privdata) {
    redisReply *reply = r;
    /*
    if (reply == NULL) return;
    printf("argv[%s]: %s, %d\n", (char*)privdata, reply->str, reply->str);
    */
    switch (reply->type)
    {
        case REDIS_REPLY_STATUS :
            printf("REDIS_REPLY_STATUS");
            break;
        case REDIS_REPLY_ERROR:
            printf("REDIS_REPLY_ERROR");
            break;
        case REDIS_REPLY_INTEGER:
            printf("REDIS_REPLY_INTEGER");
            break;
        case REDIS_REPLY_NIL:
            printf("REDIS_REPLY_NIL");
            break;
        case REDIS_REPLY_STRING:
            printf("REDIS_REPLY_STRING");
            break;
        case REDIS_REPLY_ARRAY:
            printf("REDIS_REPLY_ARRAY");
            break;
    }

    size_t i;
    for (i = 0; i < reply->elements; i++)
      printf("%d, %s\n", i, reply->element[i]->str);

    /* Disconnect after receiving the reply to GET */
    //freeReplyObject(reply);
    redisAsyncDisconnect(c);
}

void connectCallback(const redisAsyncContext *c, int status) {
    if (status != REDIS_OK) {
        printf("Error: %s\n", c->errstr);
        return;
    }
    printf("Connected...\n");
}

void disconnectCallback(const redisAsyncContext *c, int status) {
    if (status != REDIS_OK) {
        printf("Error: %s\n", c->errstr);
        return;
    }
    printf("Disconnected...\n");
}

int main (int argc, char **argv) {
    signal(SIGPIPE, SIG_IGN);
    struct event_base *base = event_base_new();

    redisAsyncContext *c = redisAsyncConnect("192.168.1.12", 9979);
    if (c->err) {
        /* Let *c leak for now... */
        printf("Error: %s\n", c->errstr);
        return 1;
    }

    redisLibeventAttach(c,base);
    redisAsyncSetConnectCallback(c,connectCallback);
    redisAsyncSetDisconnectCallback(c,disconnectCallback);
    //redisAsyncCommand(c, NULL, NULL, "SET %s %s", argv[1], argv[2]);
    //redisAsyncCommand(c, getCallback, (char*)"end-1", "GET %s", argv[1]);
    redisAsyncCommand(c, NULL, NULL, "SELECT %d", 19);
    redisAsyncCommand(c, getCallback, (char*)"end-1", "HGETALL %s", "hUToken:PadaDevice28");
    event_base_dispatch(base);
    return 0;
}
