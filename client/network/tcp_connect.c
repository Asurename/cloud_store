#include "tcp_connect.h"
#include <func.h>

int tcp_connect(const char* ip,int port){
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    int fd = socket(AF_INET,SOCK_STREAM,0);
    if(fd == -1) error(1,errno,"socket");

    int ret = connect(fd,(struct sockaddr*)&addr,sizeof(struct sockaddr));
    if(ret == -1){
        error(1,errno,"tcp_connect connect");
    }

    return fd;
}
