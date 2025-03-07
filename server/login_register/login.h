#ifndef __LOGIN_H
#define __LOGIN_H

#include "../frame/cmd_tast.h"
#include <mysql/mysql.h>

int server_login1(cmd_tast *t,MYSQL* p_mysql,char **username);
int server_login2(cmd_tast *t,MYSQL* p_mysql,char *jwt);

#endif
