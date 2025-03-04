#include "lib.h"

char current_path[512];

int main()
{
    strcpy(current_path, "/james");

    Config config;
    if (parse_config("config/config.json", &config) != 0)
    {
        fprintf(stderr, "parser failed\n");
        return -1;
    }

    // 与服务器建立连接
    int cmd_fd = tcp_connect(config.client.ip, config.client.cmd_port);

    // 创建文件接收线程池
    threadpool *thp_tsf = threadpool_init(config.thread_pool.tsf_num);
    threadpool_start(thp_tsf, thp_tsf_function, (void *)thp_tsf->q);

    // 建立epoll监听
    int epfd = epoll_create1(0);
    epoll_mod(epfd, EPOLL_CTL_ADD, EPOLLIN, cmd_fd);
    struct epoll_event recv_events[RECV_EVENTS_NUM];

    epoll_mod(epfd, EPOLL_CTL_ADD, EPOLLIN, STDIN_FILENO);

    int recv_num;

    sleep(1);
    printf(ANSI_COLOR_CYAN);
    printf("[System]wellcome!\n");
    printf(ANSI_COLOR_RESET);

    printf(ANSI_COLOR_CYAN);
    printf("Cloud_disk:james  \n");
    fflush(stdout);
    printf(ANSI_COLOR_RESET);

    printf(ANSI_COLOR_CYAN);
    printf("james@ %s $  ", current_path);
    fflush(stdout);
    printf(ANSI_COLOR_RESET);

    // 开始循环
    while (1)
    {

        recv_num = epoll_wait(epfd, recv_events, RECV_EVENTS_NUM, -1);
        for (int i = 0; i < recv_num; i++)
        {
            if (recv_events[i].data.fd == cmd_fd)
            {
                // 进入消息接收
                clinet_msg_recv(cmd_fd, current_path);
                printf(ANSI_COLOR_CYAN);
                printf("james@ %s $  ", current_path);
                fflush(stdout);
                printf(ANSI_COLOR_RESET);
            }
            else if (recv_events[i].data.fd == STDIN_FILENO)
            {
                // 进入消息发送
                client_msg_sent(cmd_fd, thp_tsf, current_path);
            }
        }
    }
}
