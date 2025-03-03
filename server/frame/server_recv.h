#ifndef __SERVER_RECV_H
#define __SERVER_RECV_H
#include "pthreadpool.h"
#include <mysql/mysql.h>
#include "../user_table/user_table.h"
#include "../threadpoll_function/thp_dct_function.h"

int server_user_recv(int listenfd, int epfd, MYSQL* p_mysql,struct user_table * userTable);
int server_msg_recv(int fd, int epfd, threadpool* thp_cmd,threadpool* thp_tsf,struct user_table * userTable, int tsf_fd);
#endif
