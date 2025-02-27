#include "server_cmd.h"
int cmd_notcmd(cmd_tast* t,MYSQL* p_mysql){
    strcpy(t->content,"CMD_TYPE_NOCMD");
    return 0;
}
