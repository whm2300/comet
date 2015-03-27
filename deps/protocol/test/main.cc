#include <stdio.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <assert.h>

#include "handle_account.h"

//#define URL "uac.api.tdd.eaglenet.cn"

int main(int argc, char *argv[])
{
    struct hostent *h;
    char **pptr;
    char str[32];
    if ((h = gethostbyname(HOST_URL)) == NULL){
        printf("error\n");
        return 1;
    }

    switch (h->h_addrtype)
    {
        case AF_INET:
        case AF_INET6:
            pptr = h->h_addr_list;
            for(; *pptr != NULL; pptr++){
                inet_ntop(h->h_addrtype, *pptr, str, sizeof(str));
                printf("addr:%s\n", str);
            }
            inet_ntop(h->h_addrtype, h->h_addr, str, sizeof(str));
            printf("first addr:%s\n", str);
            break;
        default:
            printf("error type\n");
            break;
    }

    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    assert(sock_fd > 0);

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(80);
    servaddr.sin_addr.s_addr = inet_addr(str);
    if (connect(sock_fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
      printf("connect error\n");

    HandleAccount a;
    char buffer[1024];
    memset(buffer, 0, 1024);
    int len = a.get_login_req_data(20000016, "fce92ad5c6aa0f8e2d11d2c0868ec2bd", buffer);
    printf("send---------------:\n");
    if (send(sock_fd, buffer, len, 0) != len)
      printf("send error\n");
    else
      printf("\nsend ok\n");

    memset(buffer, 0, 1024);
    len = recv(sock_fd, buffer, 1024, 0);
    bool r = a.analyse_login_data(buffer, len);
    if (r)
      printf("exist\n");
    else
      printf("not exist\n");


    return 0;
}
