#include "server_cmd.h"
int cmd_rmdir(cmd_tast* t,MYSQL* p_mysql){
    strcpy(t->content,"CMD_TYPE_RMDIR");
    return 0;
}
