#include "server_cmd.h"
int cmd_mkdir(cmd_tast* t,MYSQL* p_mysql){
    strcpy(t->content,"CMD_TYPE_MKDIR");
    return 0;
}
