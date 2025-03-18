#ifndef __LOGIN_H
#define __LOGIN_H

#include "../frame/cmd_tast.h"
void login_01(cmd_tast* t,int* connect_fd,char *username);
void login_02(cmd_tast* t,int* connect_fd,char *username,char *jwt);
void crypt_password(cmd_tast* t,char buf[],int len);

//读取用户的输入相关
int input_client_info(const char * logintips,char *buffer,int length,int type);


//纠错
void error(int status, int err, const char *fmt,...);

#endif
