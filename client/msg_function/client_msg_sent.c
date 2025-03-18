#include "client_msg_sent.h"
#include "../frame/cmd_tast.h"
#include "../config/config.h"
#include <func.h>

CMD command_type(char* token);


int client_msg_sent(int fd, threadpool* thp_tsf, char* current_path){
    //初始化用户输入缓冲区
    char buffer1[MAX_CMD_SIZE];
    char buffer2[MAX_CMD_SIZE];
    memset(buffer1,0,MAX_CMD_SIZE);
    memset(buffer1,0,MAX_CMD_SIZE);

    //初始化cmd_tast结构体
    cmd_tast *t = (cmd_tast*)malloc(sizeof(cmd_tast));
    memset(t,0,sizeof(cmd_tast));

    //接收用户输入 
    fgets(buffer1, MAX_CMD_SIZE, stdin);
    buffer1[strcspn(buffer1, "\n")] = '\0';  // 去除换行符
    
    //填充content
    strncpy(t->content, buffer1, strlen(buffer1));
    //填充cmdType
    char* token = strtok(buffer1, " ");
    if(token != NULL) {
        strncpy(buffer2, token, MAX_CMD_SIZE-1);
        buffer2[MAX_CMD_SIZE-1] = '\0';  // 确保终止符
    }
    t->cmdType = command_type(buffer2);
    //填充path
    strncpy(t->path, current_path, sizeof(t->path)-1);
    t->path[sizeof(t->path)-1] = '\0';


    //发送cmd_tast给服务器
    int ret = send(fd,t,sizeof(cmd_tast),0);

    //如果download和upload命令，扔进客户端的任务队列
    if(t->cmdType == CMD_TYPE_DOWNLOAD || t->cmdType == CMD_TYPE_DOWNLOAD_BG || t->cmdType == CMD_TYPE_UPLOAD){
        //将cmd_tast结构体扔进任务队列
        tast_queue_push(thp_tsf->q,(void*)t);
    }
    if(ret == -1) error(1,errno,"client_msg_sent send");

    return 0;
}

//判断命令的枚举类型
CMD command_type(char* token) {
    if (strcmp(token, "ls") == 0) {
        return CMD_TYPE_LS;
    }
    if (strcmp(token, "pwd") == 0) {
        return CMD_TYPE_PWD;
    }
    if (strcmp(token, "cd") == 0) {
        return CMD_TYPE_CD;
    }
    if (strcmp(token, "download") == 0) {
        return CMD_TYPE_DOWNLOAD;
    }
    if (strcmp(token, "download_bg") == 0) {
        return CMD_TYPE_DOWNLOAD_BG;
    }
    if (strcmp(token, "upload") == 0) {
        return CMD_TYPE_UPLOAD;
    }
    if (strcmp(token, "mkdir") == 0) {
        return CMD_TYPE_MKDIR;
    }
    if (strcmp(token, "rmdir") == 0) {
        return CMD_TYPE_RMDIR;
    }
    if (strcmp(token, "rm") == 0) {
        return CMD_TYPE_RM;
    }
    return CMD_TYPE_NOTCMD;
}


