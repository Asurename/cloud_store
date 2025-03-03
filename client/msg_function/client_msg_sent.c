#include "client_msg_sent.h"
#include "../frame/cmd_tast.h"
#include "../config/config.h"
#include <func.h>

CMD command_type(char* token);

//切割函数
void input_to_cmd_tast(char* input,cmd_tast* t){
    char cmd_buf[MAX_CMD_SIZE];
    char arg_buf[MAX_CMD_SIZE] = {0};
    
    // 复制输入避免修改原始数据
    strncpy(cmd_buf, input, MAX_CMD_SIZE-1);
    cmd_buf[MAX_CMD_SIZE-1] = '\0';

    // 分割命令和参数
    char* space = strchr(cmd_buf, ' ');
    if (space != NULL) {
        *space = '\0';          // 截断命令部分
        strncpy(arg_buf, space+1, MAX_CMD_SIZE-1);
        arg_buf[MAX_CMD_SIZE-1] = '\0'; // 确保终止符
    }

    // 设置结构体成员
    t->cmdType = command_type(cmd_buf);
    strncpy(t->content, arg_buf, MAX_CMD_SIZE-1);
    t->content[MAX_CMD_SIZE-1] = '\0';
}

int client_msg_sent(int fd, threadpool* thp_tsf){
    //初始化用户输入缓冲区
    char input[MAX_CMD_SIZE];
    memset(input,0,MAX_CMD_SIZE);

    //初始化cmd_tast结构体
    cmd_tast *t = (cmd_tast*)malloc(sizeof(cmd_tast));

    // 读取整行输入（包含空格）
    if (!fgets(input, MAX_CMD_SIZE, stdin)) {
        error(1, errno, "fgets failed");
    }
    input[strcspn(input, "\n")] = '\0'; // 去除换行符

    //切割函数，把接受到的字符串input解析并装入到cmd_tast结构体中
    input_to_cmd_tast(input, t);


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

