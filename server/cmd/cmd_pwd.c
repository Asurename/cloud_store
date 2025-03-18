#include "server_cmd.h"
int cmd_pwd(cmd_tast *t, MYSQL *p_mysql)
{
    strcpy(t->content, t->path);
    t->Is_printf = 1;
    return 0;
}
