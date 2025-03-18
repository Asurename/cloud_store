#include "server_cmd.h"
int cmd_notcmd(cmd_tast *t, MYSQL *p_mysql)
{
    t->Is_printf = 1;
    strcpy(t->content, "This is not valid command.");
    return 0;
}
