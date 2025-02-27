#include "server_cmd.h"
int cmd_pwd(cmd_tast* t,MYSQL* p_mysql){
    strcpy(t->content,"CMD_TYPE_PWD");
    return 0;
}
