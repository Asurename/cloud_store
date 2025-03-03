#ifndef __CLIENT_MSG_SENT_H
#define __CLIENT_MSG_SENT_H
#include "../frame/pthreadpool.h"
int client_msg_sent(int fd, threadpool* thp_tsf, char* current_path);


#endif
