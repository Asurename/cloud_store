#include "../frame/cmd_tast.h"
#include "../frame/pthreadpool.h"
#include <mysql/mysql.h>
#include "../mysql/mysql_io.h"
#include "../cmd/server_cmd.h"
#include "../login_register/login.h"
#include "../login_register/register.h"

void* thp_cmd_function(void * arg){
    //------------------------------------------------//
    //预处理区
    //t是客户端传来的cmd_tast
    tast_queue* q = (tast_queue*)arg;
    cmd_tast* t;

    int t_len = sizeof(cmd_tast);
    MYSQL* p_mysql;
    p_mysql = mysql_disk_connect(MYSQL_IP, MYSQL_PORT, MYSQL_NAME, MYSQL_PASSWD, MYSQL_DB);
    char username[1024];
    char jwt[4096];
    //------------------------------------------------//
    //命令处理区
    while (1) {
        memset(username,0,sizeof(username));
        memset(jwt,0,sizeof(jwt));
        t = (cmd_tast*)tast_queue_pop(q);
        int send_fd = t->peerfd;

        //------------------------------------------------//
            //解析命令区
            //这里会对t的path于content进行解析于修改
        printf("type:%d\n",t->cmdType);
        switch (t->cmdType) {
        case CMD_TYPE_LS:
            t->Is_printf = 1;
            cmd_ls(t,p_mysql);
            break;

        case CMD_TYPE_CD:
            t->Is_printf = 1;
            cmd_cd(t,p_mysql);
            break;

        case CMD_TYPE_PWD:
            t->Is_printf = 1;
            cmd_pwd(t,p_mysql);
            break;

        case CMD_TYPE_MKDIR:
            cmd_mkdir(t,p_mysql);
            break;

        case CMD_TYPE_RMDIR:
            cmd_rmdir(t, p_mysql);
            break;

        case CMD_TYPE_RM:
            cmd_rm(t,p_mysql);
            break;

        case CMD_TYPE_NOTCMD:
            t->Is_printf = 1;
            cmd_notcmd(t,p_mysql);
            break;
        case CMD_TYPE_REGIT1:
            server_regite1(t,p_mysql);
            break;
        case CMD_TYPE_REGIT2:
             server_regite2(t,p_mysql);
            break;
        case CMD_TYPE_LOGIN1:
            server_login1(t,p_mysql,&username);
            printf("username : %s\n",username);
            break;
        case CMD_TYPE_LOGIN2:
            printf("-------------");
            encode(username,&jwt);
            printf("---------------------------------");
            printf("jwt : %s\n",jwt);
            server_login2(t,p_mysql,jwt);
            break;

        }
        //------------------------------------------------//
            //回信打包区
        int err1 = send(send_fd, (char*)t, t_len, 0);
        if (err1 == -1) {
            error(0, errno, "send");
        }

        free(t);
    }
    return NULL;
}
