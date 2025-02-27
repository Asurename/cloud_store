#ifndef __SERVER_RECV_H
#define __SERVER_RECV_H
#include "pthreadpool.h"
#include <mysql/mysql.h>

int server_user_recv(int listenfd, int epfd, MYSQL* p_mysql);
int server_msg_recv(int fd, int epfd, threadpool* thp_cmd,threadpool* thp_tsf);
#endif
