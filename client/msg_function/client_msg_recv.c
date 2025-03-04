#include "client_msg_recv.h"
#include "../frame/cmd_tast.h"
#include <func.h>

int clinet_msg_recv(int fd, char *current_path)
{
    // 创建cmd_tast结构体
    cmd_tast *t = (cmd_tast *)malloc(sizeof(cmd_tast));
    int ret = recv(fd, t, sizeof(cmd_tast), 0);
    // 异常检测
    if (ret == -1)
    {
        error(0, errno, "clinet_msg_recv recv");
        return -1;
    }

    if (ret == 0)
    {
        error(1, errno, "被踢出");
        return -1;
    }

    strcpy(current_path, t->path);

    // 检测是否要打印文字
    if (t->Is_printf == 1)
    {
        printf("%s\n", t->content);
    }
    return 0;
}
