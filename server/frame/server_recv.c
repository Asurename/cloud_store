#include "server_recv.h"
#include <func.h>
#include "../config/config.h"
#include "epoll_plug.h"
#include "cmd_tast.h"

int server_user_recv(int listenfd, int epfd, MYSQL* p_mysql){
    //对新用户进行accept
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int fd = accept(listenfd,(struct sockaddr*)&client_addr,&client_addr_len);
    printf(ANSI_COLOR_CYAN);
    printf("[NEW_USER]Wellcome user:%d\n",fd);
    printf(ANSI_COLOR_RESET);

    //对新用户进行监听
    epoll_mod(epfd,EPOLL_CTL_ADD,EPOLLIN,fd);
    return 0;
}

int server_msg_recv(int fd, int epfd, threadpool* thp_cmd,threadpool* thp_tsf){
    //创建接收结构体
    cmd_tast* recv_t = (cmd_tast*)malloc(sizeof(cmd_tast));
    int ret = recv(fd,(char*)recv_t,sizeof(cmd_tast),0);

    //异常情况处理
    if(ret == -1){
        error(0,errno,"server_msg_recv recv");
        return -1;
    }else if(ret == 0){
        printf(ANSI_COLOR_RED);
        printf("[USER_EXIT]User %d exit\n",fd);
        printf(ANSI_COLOR_RESET);
        close(fd);
        epoll_mod(epfd,EPOLL_CTL_DEL,EPOLLIN,fd);
        free(recv_t);
        return -1;
    }
    
    recv_t->peerfd = fd;
    if(recv_t->cmdType == CMD_TYPE_DOWNLOAD ||recv_t->cmdType == CMD_TYPE_UPLOAD||
       recv_t->cmdType == CMD_TYPE_DOWNLOAD_BG){

        tast_queue_push(thp_tsf->q,(void*)recv_t);
    }else{
        tast_queue_push(thp_cmd->q,(void*)recv_t);
    }

    return 0;
}

