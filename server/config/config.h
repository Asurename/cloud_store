#ifndef __CONFIG_H
#define __CONFIG_H

#include <func.h>
#include <cjson/cJSON.h>


typedef struct {
    char ip[128];
    int cmd_port;
    int tsf_port;
} ServerConfig;

typedef struct {
    int cmd_num;
    int tsf_num;
} ThreadPoolConfig;

typedef struct {
    char ip[128];
    int port;
    char username[64];
    char password[64];
    char database[64];
} MySQLConfig;

typedef struct {
    ServerConfig server;
    ThreadPoolConfig thread_pool;
    MySQLConfig mysql;
} Config;


//服务器监听IP与端口
// #define IP "10.1.27.255"
// #define PORT_CMD 13333
// #define PORT_TSF 12222

//线程池启动数量
// #define THP_CMD_NUM 4
// #define THP_TSF_NUM 4

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
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW   "\x1b[33m"
#define ANSI_COLOR_BULE   "\x1b[34m"
#define ANSI_COLOR_HIGHLIGHT   "\x1b[1m"
#define ANSI_COLOR_START   "\x1b[5m"
#define ANSI_COLOR_NOSTART   "\x1b[25m"
#define ANSI_COLOR_PURPLE   "\x1b[35m"


 char *read_file(const char *filename);
 int parse_config(const char *filename, Config *config);
#endif
