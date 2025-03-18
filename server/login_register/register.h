#ifndef __REGISTER_H
#define __REGISTER_H
#include <mysql/mysql.h>
#include "../frame/cmd_tast.h"

int server_regite1(cmd_tast* t,MYSQL* p_mysql);
int server_regite2(cmd_tast* t,MYSQL* p_mysql);
#endif
