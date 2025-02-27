#ifndef __SERVER_CMD_H
#define __SERVER_CMD_H

#include <func.h>
#include <mysql/mysql.h>
#include "../frame/cmd_tast.h"
//命令处理函数合集
int cmd_ls(cmd_tast* t,MYSQL* p_mysql);
int cmd_pwd(cmd_tast* t,MYSQL* p_mysql);
int cmd_cd(cmd_tast* t,MYSQL* p_mysql);
int cmd_mkdir(cmd_tast* t,MYSQL* p_mysql);
int cmd_rmdir(cmd_tast* t,MYSQL* p_mysql);
int cmd_rm(cmd_tast* t,MYSQL* p_mysql);
int cmd_notcmd(cmd_tast* t,MYSQL* p_mysql);
#endif
