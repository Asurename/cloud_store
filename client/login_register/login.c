#include "login.h"
#include <stdarg.h>

// 使用extern声明current_path
extern char current_path[512]; 

// 自定义的error函数
void error(int status, int err, const char *fmt,...) {
    va_list args;
    va_start(args, fmt);

    // 如果err不为0，输出错误码和对应的系统错误信息
    if (err != 0) {
        fprintf(stderr, "Error %d: %s - ", err, strerror(err));
    }

    // 输出自定义的错误信息
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");

    va_end(args);

    // 如果status不为0，终止程序
    if (status != 0) {
        exit(status);
    }
}

// 清空输入缓冲区
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

//login1流程
//输入用户名-----将输入内容发给服务端-----服务端返回结果-----根据结果判断是否继续输入---也就是判断用户名是否正确
void login_01(cmd_tast* t,int* connect_fd,char *username){
    while(1){
        memset(t->content,0,MAX_CMD_SIZE);
        t->cmdType=CMD_TYPE_LOGIN1;
        char buffer[MAX_CMD_SIZE];

        //printf(ANSI_COLOR_CYAN);
        //printf("请输入用户名:");
        //printf(ANSI_COLOR_RESET);

        // 清空输入缓冲区
        //clear_input_buffer();

        //处理错误
        if(input_client_info("请输入用户名:",buffer,MAX_CMD_SIZE,1)==-1){
            error(0,errno,"input_client_info failed in login_01 ");
            continue; 
        }
        //存到结构体中
        strcpy(t->content,buffer);

        strcpy(username,buffer);
        //这时候t->content里就是用户名
        //printf("t->content in client_login01:%s\n",t->content);

        send(*connect_fd,t,sizeof(cmd_tast),0);

        int err=recv(*connect_fd,(char*)t,sizeof(cmd_tast),MSG_WAITALL);
        if(err==-1){
            error(0,errno,"recv failed in login_01");
            continue;
        }

        //printf("input username = %s\n",buffer);
        
        //成功登录
        if(t->cmdType==CMD_TYPE_LOGIN1_OK){
            printf(ANSI_COLOR_CYAN);
            printf("[System]");
            printf(ANSI_COLOR_RESET);
            printf("用户名正确请继续\n");
            break;
        }
        //没成功
        else if(t->cmdType==CMD_TYPE_LOGIN1_ERROR){
            printf(ANSI_COLOR_CYAN);
            printf("[System]");
            printf(ANSI_COLOR_RESET);
            printf("用户名错误请重试\n");
            continue;
        }
    }
}
//login2流程
//基于第一步获取盐值----对用户输入的密码进行加密-----发送给服务器----根据服务器响应是否成功
void login_02(cmd_tast* t,int* connect_fd,char *username,char *jwt){
     //存盐值
     char buffersalt[4096]={0};
     strncpy(buffersalt,t->content,strlen(t->content));
     buffersalt[sizeof(buffersalt) - 1] = '\0';
 
     while(1){
         memset(t->content,0,MAX_CMD_SIZE);
         t->cmdType=CMD_TYPE_LOGIN2;
         //存密码
         char bufferpassword[MAX_CMD_SIZE]={0};
         printf(ANSI_COLOR_CYAN);
         input_client_info("请输入密码:",bufferpassword,sizeof(bufferpassword),0);

         printf(ANSI_COLOR_RESET);
         //加密
         char* crypted_password=crypt(bufferpassword,buffersalt);
         //printf("pw = %s\n",bufferpassword);
         //printf("buffersalt = %s\n",buffersalt);
         //printf("crypted_password = %s\n",crypted_password);
         if(crypted_password==NULL){
             error(0,errno,"crypt failed in login_02");
             continue; 
         }
         //清空t->content.将加密后的密码赋值到t->content
         memset(&t->content,0,MAX_CMD_SIZE);
         strcpy(t->content,crypted_password);
         //发送
         send(*connect_fd,t,sizeof(cmd_tast),0);
         //接收
         int err=recv(*connect_fd,(char*)t,sizeof(cmd_tast),MSG_WAITALL);
         if(err==-1){
             error(0,errno,"recv failed in login_02");
             continue;
         }
         //成功登录
         if(t->cmdType==CMD_TYPE_LOGIN2_OK){
             printf(ANSI_COLOR_CYAN);
             printf("[System]");
             printf("成功登录\n");
             printf(ANSI_COLOR_HIGHLIGHT);
             printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
             printf(ANSI_COLOR_RESET);
             // 更新 current_path
            //printf("current_path is = %s\n",current_path);
            snprintf(current_path, sizeof(current_path), "/%s", username);
            //printf("%s@1111 /%s $",username, username);  // 使用保存的用户名
            //printf("[DEBUG] t->remain address: %p, content: '%s'\n", t->remain, t->remain);
            //printf("[DEBUG] jwt address: %p\n", jwt);
            //t->remain[sizeof(t->remain) - 1] = '\0';  
            strncpy(jwt,t->remain,sizeof(t->remain));
            //printf("jwt : %s,t->remain : %s\n",jwt,t->remain);
             break;
         }
         if(t->cmdType==CMD_TYPE_LOGIN2_ERROR){
             printf(ANSI_COLOR_RED);
             printf("[System]");
             printf(ANSI_COLOR_RESET);
             printf("密码错误请重试\n");
             continue;
         }
     }
}
void crypt_password(cmd_tast* t,char buf[],int len){
    return;
}

