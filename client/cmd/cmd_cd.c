#include "server_cmd.h"
int cmd_cd(cmd_tast* t,MYSQL* p_mysql){
    strcpy(t->content,"CMD_TYPE_CD");
    return 0;
}
