#ifndef __REGISTER_H
#define __REGISTER_H
#include "../frame/cmd_tast.h"
#define MAX_USERNAME_LEN  20 
#define MAX_PASSWORD_LEN  20 
int client_regite1(int connect_fd,cmd_tast* t);
int client_regite2(int connect_fd, cmd_tast* t);

//读取用户的输入相关
int input_client_info(const char * logintips,char *buffer,int length,int type);

//纠错
void error(int status, int err, const char *fmt,...);
#endif
