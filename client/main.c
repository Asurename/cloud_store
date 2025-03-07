#include "lib.h"
#include "login_register/login.h"
#include "login_register/register.h"
char current_path[512];
// 封装打印提示信息的函数
char username[MAX_CMD_SIZE];
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

void print_line();

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
    //threadpool_start(thp_tsf, thp_tsf_function, (void *)thp_tsf);
    
    //初始化接收状态
    char path[PATH_SIZE] = {'\0'};
    strcpy(path,"home");

    cmd_tast t_send;
    strcpy(t_send.path,path);

    cmd_tast t_recv;
    int t_len = sizeof(cmd_tast);
    char buf[MAX_CMD_SIZE];


    // 建立epoll监听
    int epfd = epoll_create1(0);
    epoll_mod(epfd, EPOLL_CTL_ADD, EPOLLIN, cmd_fd);
    struct epoll_event recv_events[RECV_EVENTS_NUM];

    epoll_mod(epfd, EPOLL_CTL_ADD, EPOLLIN, STDIN_FILENO);

    //char jwt[4096] = {0};
    int recv_num;
    int choice_made = 0;  // 标记是否已经做出选择
    system("clear");

    printf(ANSI_COLOR_HIGHLIGHT);
    printf(ANSI_COLOR_YELLOW);
    printf("------------------------------------------\n");
    printf("\t欢迎使用new组云服务器\n");
    printf("\t请先完成登入或注册\n");
    printf("\tVIP 30/月 享受高速下载专线\n");
    printf("\t首次购买享受八折优惠！(๑´ڡ`๑)\n");
    printf("------------------------------------------\n");
    printf(ANSI_COLOR_RESET);
    sleep(1);

    while (!choice_made) {
        print_prompt("Input << 1 >>  Login");
        print_prompt("Input << 2 >>  Registration");

        char c = get_valid_input();

        if (c == '1') {
            login_01(&t_send, &cmd_fd,username);
            //printf("[DEBUG] jwt address: %p\n", thp_tsf->jwt);
            login_02(&t_send, &cmd_fd,username,thp_tsf->jwt);
            //strcpy(&(thp_tsf->jwt),jwt);
            //printf("thp-tsf->jwt : %s\n",thp_tsf->jwt);
            strcpy(path, t_send.path);
            choice_made = 1; // 标记已经做出选择
        } else if (c == '2') {
            client_regite1(cmd_fd, &t_send);
            client_regite2(cmd_fd, &t_send);
            strcpy(path, t_send.path);
            choice_made = 1; // 标记已经做出选择
        }
    }
    threadpool_start(thp_tsf, thp_tsf_function, (void *)thp_tsf);
    sleep(1);

    printf(ANSI_COLOR_YELLOW);
    printf(ANSI_COLOR_HIGHLIGHT);
    printf("\n------------------------------------------\n");
    printf(ANSI_COLOR_START);
    printf("\t[System]wellcome %s!(ゝ∀･)\n",username);
    printf(ANSI_COLOR_NOSTART);
    printf("------------------------------------------\n\n");
    printf(ANSI_COLOR_RESET);

    print_line();

    printf(ANSI_COLOR_HIGHLIGHT);
    printf("%s[%s(=´ω`=)]%s %s >> %s",ANSI_COLOR_CYAN,username,
                       ANSI_COLOR_YELLOW, current_path,ANSI_COLOR_RESET);
    fflush(stdout);


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
                printf(ANSI_COLOR_HIGHLIGHT);
                printf("%s[%s(=´ω`=)]%s %s >> %s",ANSI_COLOR_CYAN,username,
                       ANSI_COLOR_YELLOW, current_path,ANSI_COLOR_RESET);
                fflush(stdout);
            }
            else if (recv_events[i].data.fd == STDIN_FILENO)
            {
                // 进入消息发送
                client_msg_sent(cmd_fd, thp_tsf, current_path);
            }
        }
    }
}


void print_line(){
    char arr[5][4096];
    for(int i = 0;i<5;i++){
        memset(arr[i],0,4096);
    }
    strcpy(arr[0],"[写命令和文件系统的james]2025湖人总冠军！！！！！");
    strcpy(arr[1],"[摸鱼的suis]甜豆腐脑是对的，咸豆腐脑是错的");
    strcpy(arr[2],"[写文件传输的dsw]广告位待租，价优，诚邀合作!");
    strcpy(arr[3],"[脑袋尖尖的wzh]那我问你?盐值是男的还是女的?");
    strcpy(arr[4],"[写命令的Luxun]大抵是我的口袋清高了，世俗的铜臭味已经入不了它的世界。");
    
    srand((unsigned)time(NULL));
    int seed = rand();
    printf("%s%sTips:%s%s\n\n",ANSI_COLOR_HIGHLIGHT,
           ANSI_COLOR_PURPLE,arr[seed%5],
           ANSI_COLOR_RESET);
}
