#include <stdio.h>
#include <func.h>
#include "lib.h"

int main(){

    openlog("server::", LOG_PID|LOG_CONS, LOG_LOCAL0);

    Config config;
    if (parse_config("config/config.json", &config) != 0)
    {
        syslog(LOG_ERR,"parser failed\n");
        fprintf(stderr, "parser failed\n");
        return -1;
    }

    //建立网络连接
    int cmd_fd = tcp_connection_bliud(config.server.ip,config.server.cmd_port);
    int tsf_fd = tcp_connection_bliud(config.server.ip,config.server.tsf_port);

    MYSQL* p_mysql;
    p_mysql = mysql_disk_connect(config.mysql.ip,config.mysql.port,config.mysql.username,config.mysql.password,config.mysql.database);

    //初始化线程池
    threadpool* thp_cmd = threadpool_init(config.thread_pool.cmd_num);
    threadpool* thp_tsf = threadpool_init(config.thread_pool.tsf_num);
    threadpool* thp_dct = threadpool_init(1);
    struct user_table *ut = user_table_init();

    // 线程池传入参数
    queue_and_mysql_t* qm = (queue_and_mysql_t*)malloc(sizeof(queue_and_mysql_t));
    qm->q = thp_tsf->q;
    qm->p_mysql = p_mysql;
    
    //线程池启动
    threadpool_start(thp_cmd,thp_cmd_function,(void*)thp_cmd->q);
    threadpool_start(thp_tsf,thp_tsf_function,(void*)qm);
    threadpool_start(thp_dct,thp_dct_function,(void*)ut);

    //建立epoll监听
    int ep_fd = epoll_create1(0);
    epoll_mod(ep_fd,EPOLL_CTL_ADD,EPOLLIN,cmd_fd);
    epoll_mod(ep_fd,EPOLL_CTL_ADD,EPOLLIN,tsf_fd);
    struct epoll_event recv_events[RECV_EVENTS_NUM];

    int recv_num;
    sleep(1);
    syslog(LOG_INFO,"[System]The server is ready...\n");
    printf("[System]The server is ready...\n");
    while(1){
        recv_num = epoll_wait(ep_fd,recv_events,RECV_EVENTS_NUM,-1);
        for(int i = 0;i<recv_num;i++){
            if(recv_events[i].data.fd == cmd_fd){
                //进行新用户的接收
                server_user_recv(cmd_fd,ep_fd,p_mysql,ut);
            }else{
                server_msg_recv(recv_events[i].data.fd,ep_fd,thp_cmd,thp_tsf,ut, tsf_fd);
            }
        }
    }
}


