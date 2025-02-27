#include "client_msg_sent.h"
#include "../frame/cmd_tast.h"
#include "../config/config.h"
#include <func.h>

CMD command_type(char* token);


int client_msg_sent(int fd){
    //初始化用户输入缓冲区
    char buffer1[MAX_CMD_SIZE];
    char buffer2[MAX_CMD_SIZE];
    memset(buffer1,0,MAX_CMD_SIZE);
    memset(buffer2,0,MAX_CMD_SIZE);
    //用户输入
    scanf("%s",buffer1);
    //初始化cmd_tast结构体
    cmd_tast *t = (cmd_tast*)malloc(sizeof(cmd_tast));
    strcpy(t->content,buffer2);
    t->cmdType = command_type(buffer1);
    //发送cmd_tast给服务器
    int ret = send(fd,t,sizeof(cmd_tast),0);
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
