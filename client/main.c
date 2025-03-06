#include "lib.h"
#include "login_register/login.h"
#include "login_register/register.h"
char current_path[512];
// 封装打印提示信息的函数
void print_prompt(const char* message) {
    printf(ANSI_COLOR_CYAN);
    printf("[System]");
    printf(ANSI_COLOR_RESET);
    printf("%s\n", message);
}

// 封装获取用户输入并验证的函数
char get_valid_input() {
    char c;
    while (1) {
        c = getchar();
        // 清除输入缓冲区
        while (getchar() != '\n');
        if (c == '1' || c == '2') {
            return c;
        } else {
            print_prompt("Invalid input. Please input 1 for Login or 2 for Registration.");
        }
    }
}


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
    if (cmd_fd == -1) {
        // 连接服务器失败，输出错误信息并返回 -1
        perror("tcp_connect failed");
        return -1;
    }

    // 创建文件接收线程池
    threadpool *thp_tsf = threadpool_init(config.thread_pool.tsf_num);
    if (thp_tsf == NULL) {
        // 线程池初始化失败，输出错误信息并返回 -1
        perror("threadpool_init failed");
        close(cmd_fd);
        return -1;
    }
    threadpool_start(thp_tsf, thp_tsf_function, (void *)thp_tsf->q);
    
    //初始化接收状态
    char path[PATH_SIZE] = {'\0'};
    strcpy(path,"home");

    cmd_tast t_send;
    strcpy(t_send.path,path);

    cmd_tast t_recv;
    int t_len = sizeof(cmd_tast);
    char buf[MAX_CMD_SIZE];

    char username[MAX_CMD_SIZE];

    // 建立epoll监听
    int epfd = epoll_create1(0);
    epoll_mod(epfd, EPOLL_CTL_ADD, EPOLLIN, cmd_fd);
    struct epoll_event recv_events[RECV_EVENTS_NUM];

    epoll_mod(epfd, EPOLL_CTL_ADD, EPOLLIN, STDIN_FILENO);

    int recv_num;
    int choice_made = 0;  // 标记是否已经做出选择
    while (!choice_made) {
        print_prompt("Input << 1 >>  Login");
        print_prompt("Input << 2 >>  Registration");

        char c = get_valid_input();

        if (c == '1') {
            login_01(&t_send, &cmd_fd,username);
            login_02(&t_send, &cmd_fd,username);
            strcpy(path, t_send.path);
            choice_made = 1; // 标记已经做出选择
        } else if (c == '2') {
            client_regite1(cmd_fd, &t_send);
            client_regite2(cmd_fd, &t_send);
            strcpy(path, t_send.path);
            choice_made = 1; // 标记已经做出选择
        }
    }
    sleep(1);
    printf(ANSI_COLOR_CYAN);
    printf("[System]wellcome!\n");
    printf(ANSI_COLOR_RESET);

    printf(ANSI_COLOR_CYAN);
    printf("Cloud_disk:%s  \n",current_path);
    fflush(stdout);
    printf(ANSI_COLOR_RESET);

    printf(ANSI_COLOR_CYAN);
    printf("%s@ %s $  ",username, current_path);
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
                printf("%s@ %s $  ",current_path, current_path);
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