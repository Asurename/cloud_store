#include "server_cmd.h"
int cmd_rm(cmd_tast* t,MYSQL* p_mysql){
    strcpy(t->content,"CMD_TYPE_RM");
    return 0;
}
