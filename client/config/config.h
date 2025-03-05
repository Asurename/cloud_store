#ifndef __CONFIG_H
#define __CONFIG_H

#include <func.h>
#include <cjson/cJSON.h>

typedef struct {
    char ip[128];
    int cmd_port;
    int tsf_port;
} clientConfig;

typedef struct {
    int tsf_num;
} ThreadPoolConfig;

typedef struct {
    clientConfig client;
    ThreadPoolConfig thread_pool;
} Config;

//服务器监听IP与端口
// #define IP "192.168.182.130"
// #define PORT_CMD 13333
// #define PORT_TSF 12222

//epoll监听最大接受数量
#define RECV_EVENTS_NUM 4096

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
