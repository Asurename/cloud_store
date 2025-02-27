#include "server_cmd.h"
int cmd_ls(cmd_tast* t,MYSQL* p_mysql){
    strcpy(t->content,"CMD_TYPE_LS");
    return 0;
}
