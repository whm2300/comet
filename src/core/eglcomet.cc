#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

bool to_daemon()
{
    do
    {
        switch (fork()){
            case -1 :
                return false;
            case 0 :
                break;
            default :
                exit(0);
        }
        if (setsid() == -1){
            break;
        }

        if (close(STDIN_FILENO) == -1){
            break;
        }
        if (close(STDOUT_FILENO) == -1){
            break;
        }
        if (close(STDERR_FILENO) == -1){
            break;
        }

        int fd = open("/dev/null", O_RDWR, 0);
        if (fd == -1){
            break;
        }
        if (dup2(fd, STDIN_FILENO) == -1){
            break;
        }
        if (dup2(fd, STDOUT_FILENO) == -1){
            break;
        }
        if (dup2(fd, STDERR_FILENO) == -1){
            break;
        }
        return true;
    }while (0);


    return false;
}

int main(int argc, char *argv[])
{
    if (argc != 2){
        printf("param error\n");
        return 1;
    }

    Server *server = new Server();
    {
        std::string str(argv[1]);
        server->analyse_config(str);
    }
    if (server->is_daemon() && !to_daemon()){
        printf("fork erro\n");
        exit(1);
    }

    if (!server->init_server()){
        exit(1);
    }

    server->run_server();

    delete server;
    return 0;
}
