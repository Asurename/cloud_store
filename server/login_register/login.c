#include "login.h"
#include "../mysql/mysql_io.h"
//判断用户名是否正确
int server_login1(cmd_tast *t,MYSQL* p_mysql){
    printf("LoginCheck1.\n");
    char saltbuf[256]={0};//存盐值

    printf("username entered by user: %s\n",t->content);

    //int ret = mysql_get_user_info(p_mysql, saltbuf, "username", t->content, "salt");
    int ret= mysql_get_user_data(p_mysql,saltbuf,t->content,"salt");

//1、用户不存在
    if(ret==-1)
{
    t->peerfd=0;

    char tmp[]="username error,please enter again.";
    memset(t->content,0,sizeof(t->content));
    strncpy(t->content,tmp,sizeof(t->content) - 1);
    t->content[sizeof(t->content) - 1] = '\0';

    t->cmdType=CMD_TYPE_LOGIN1_ERROR;
    return 0;
}

//2、用户存在
    t->peerfd=-1;

    memset(t->content,0,sizeof(t->content));
    strncpy(t->content,saltbuf,sizeof(t->content) - 1);
    t->content[sizeof(t->content) - 1] = '\0';

    t->cmdType=CMD_TYPE_LOGIN1_OK;
    return 0;
} 
// 判断密码是否正确
int server_login2(cmd_tast *t,MYSQL* p_mysql){
    printf("LoginCheck2.\n");
    char stored_cryptpasswd[256] = {0};  // 用于存储从数据库中获取的加密密码
    //char pathbuf[256] = {0};  
    printf("cryptpasswd =%s\n",t->content);

    // 从数据库中获取存储的加密密码
    //mysql_get_user_info(p_mysql, stored_cryptpasswd, "username", t->content, "cryptpasswd");
    mysql_get_user_path(p_mysql,stored_cryptpasswd,t->content,"cryptpasswd");

    // 调试
    printf("Client sent cryptpasswd: %s\n", t->content);
    printf("Stored cryptpasswd: %s\n", stored_cryptpasswd);

    // 1、登录失败
    if (strcmp(t->content, stored_cryptpasswd) != 0) {
        t->peerfd = 0;

        char contentbuf1[] = "Login failed.";
        memset(t->content, 0, sizeof(t->content));
        strncpy(t->content, contentbuf1, sizeof(t->content) - 1);
        t->content[sizeof(t->content) - 1] = '\0';

        t->cmdType = CMD_TYPE_LOGIN2_ERROR;

        return 0;
    }

    // 2、登录成功
    t->peerfd = 0;

    t->cmdType = CMD_TYPE_LOGIN2_OK;

    memset(&t->path, 0, sizeof(t->path));
    strncpy(t->path, stored_cryptpasswd, sizeof(t->path) - 1);
    t->path[sizeof(t->path) - 1] = '\0';

    return 0;
}
