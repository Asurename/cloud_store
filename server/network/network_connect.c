#include "../lib.h"
//服务端tcp等待连接的建立，最后返回fd
int tcp_connection_bliud(const char *ip,int port){
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    int fd = socket(AF_INET,SOCK_STREAM,0);
    if(fd == -1) error(1,errno,"socket");
    
    int ret;
    ret = bind(fd,(struct sockaddr*)&addr,sizeof(struct sockaddr));
    if(ret == -1) error(1,errno,"bind");

    ret = listen(fd,4096);
    if(ret == -1) error(1,errno,"listen");
    printf(ANSI_COLOR_CYAN);
    printf("[TCP_CONNECT]Tcp connection is ready...\n");
    printf(ANSI_COLOR_RESET);
    return fd;
}
