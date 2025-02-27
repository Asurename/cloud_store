#ifndef __CONFIG_H
#define __CONFIG_H

//服务器监听IP与端口
#define IP "10.1.24.17"
#define PORT_CMD 13333
#define PORT_TSF 12222

//线程池启动数量
#define THP_CMD_NUM 4
#define THP_TSF_NUM 4

//epoll监听最大接受数量
#define RECV_EVENTS_NUM 4096

//mysql参数
#define MYSQL_IP "42.194.149.92"
#define MYSQL_PORT 3306
#define MYSQL_NAME "root"
#define MYSQL_PASSWD "6862420ok"
#define MYSQL_DB "cloud_disk_v2"

#define MAX_CMD_SIZE 4096
#define PATH_SIZE 4096

#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#endif
