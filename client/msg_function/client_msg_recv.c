#include "client_msg_recv.h"
#include "../frame/cmd_tast.h"
#include <func.h>

int clinet_msg_recv(int fd){
    cmd_tast *t = (cmd_tast*)malloc(sizeof(cmd_tast));    
    int ret = recv(fd,t,sizeof(cmd_tast),0);

    if(ret == -1){
        error(0,errno,"clinet_msg_recv recv");
        return -1;
    }

    if(t->Is_printf == 1){
        printf("%s\n",t->content);
    }
    return 0;
}
