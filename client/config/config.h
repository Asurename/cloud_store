#ifndef __CONFIG_H
#define __CONFIG_H

//服务器监听IP与端口
#define IP "42.194.149.92"
#define PORT_CMD 13333
#define PORT_TSF 12222

//epoll监听最大接受数量
#define RECV_EVENTS_NUM 4096

#define MAX_CMD_SIZE 4096
#define PATH_SIZE 4096

#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#endif
