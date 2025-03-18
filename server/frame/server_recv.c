#include "server_recv.h"
#include <func.h>
#include "../config/config.h"
#include "epoll_plug.h"
#include "cmd_tast.h"
#include <syslog.h>

int server_user_recv(int listenfd, int epfd, MYSQL *p_mysql, struct user_table *userTable)
{
    // 对新用户进行accept
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int fd = accept(listenfd, (struct sockaddr *)&client_addr, &client_addr_len);
    printf(ANSI_COLOR_CYAN);
    syslog(LOG_INFO, "[NEW_USER]Wellcome user:%d\n", fd);
    printf("[NEW_USER]Wellcome user:%d\n", fd);
    printf(ANSI_COLOR_RESET);

    pthread_mutex_lock(&timeout_index_mutex);
    int current_index = timeoutArrayIndex;
    // printf("timeoutArrayIndex : %d \n",timeoutArrayIndex);
    pthread_mutex_unlock(&timeout_index_mutex);
    int setIdx = -1;
    timeout_array_add(netfdArray, userTable, current_index, fd, &setIdx);

    // 对新用户进行监听
    epoll_mod(epfd, EPOLL_CTL_ADD, EPOLLIN, fd);
    return 0;
}

int server_msg_recv(int fd, int epfd, threadpool *thp_cmd, threadpool *thp_tsf, struct user_table *userTable, int tsf_fd)
{
    // 创建接收结构体
    cmd_tast *recv_t = (cmd_tast *)malloc(sizeof(cmd_tast));
    int ret = recv(fd, (char *)recv_t, sizeof(cmd_tast), MSG_WAITALL);
    // 异常情况处理
    if (ret == -1)
    {
        error(0, errno, "server_msg_recv recv");
        return -1;
    }
    else if (ret == 0)
    {
        printf(ANSI_COLOR_RED);
        syslog(LOG_INFO, "[USER_EXIT]User %d exit\n", fd);
        printf("[USER_EXIT]User %d exit\n", fd);
        printf(ANSI_COLOR_RESET);
        int setIdx = -1;
        int current_index = user_table_find(fd, userTable, &setIdx);
        if (setIdx < 0)
        {
            syslog(LOG_ERR, "setIdx = -1\n");
            printf("setIdx = -1\n");
            return -1;
        }
        netfdArray[current_index][setIdx] = -1;
        user_table_erase(fd, userTable);
        close(fd);
        epoll_mod(epfd, EPOLL_CTL_DEL, EPOLLIN, fd);
        free(recv_t);
        return -1;
    }

    recv_t->peerfd = fd;
    // 把短命令和长命令扔进不同的任务队列，不同的线程池会去处理
    if (recv_t->cmdType == CMD_TYPE_DOWNLOAD || recv_t->cmdType == CMD_TYPE_UPLOAD ||
        recv_t->cmdType == CMD_TYPE_DOWNLOAD_BG)
    {
        recv_t->peerfd = tsf_fd;
        tast_queue_push(thp_tsf->q, (void *)recv_t);
        pthread_mutex_lock(&timeout_index_mutex);
        int current_index = timeoutArrayIndex;
        pthread_mutex_unlock(&timeout_index_mutex);
        timeout_array_change(netfdArray, userTable, current_index, fd);
    }
    else
    {
        tast_queue_push(thp_cmd->q, (void *)recv_t);
        pthread_mutex_lock(&timeout_index_mutex);
        int current_index = timeoutArrayIndex;
        pthread_mutex_unlock(&timeout_index_mutex);
        timeout_array_change(netfdArray, userTable, current_index, fd);
    }

    return 0;
}
