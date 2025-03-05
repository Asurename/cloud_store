#include "register.h"
// 使用extern声明current_path
extern char current_path[512]; 
//输入相关
int input_client_info(const char * logintips,char *buffer,int length){
    // 检查参数有效性
    if (logintips == NULL || buffer == NULL || length <= 0) {
        error(0, 0, "input_client_info: invalid parameters");
        return -1;
    }

    //打印提示信息
    printf("%s",logintips);
    fflush(stdout);
    //读取用户输入
    if(fgets(buffer,length,stdin)==NULL){
        error(0, errno, "inValid input");
        return -1;
    }
    //去掉换行符 如果不为空字符且最后有换行符,就替换为\0
    ssize_t input_length=strlen(buffer);
    if(input_length>0&&buffer[input_length-1]=='\n'){
        buffer[input_length-1]='\0';
    }
    return 0;
}

int client_regite1(int connect_fd,cmd_tast* t){
    char username[MAX_CMD_SIZE] = {0};
    int isUsernameAvailable = 0;

    while (!isUsernameAvailable) {
        // 初始化结构体内容
        memset(t->content, 0, MAX_CMD_SIZE);

        // 输入用户名
        t->cmdType = CMD_TYPE_REGIT1;
        if (input_client_info("请输入用户名: ", username, sizeof(username)) != 0) {
            continue; // 输入失败，重新输入
        }

        // 复制用户名到结构体字段
        strcpy(t->content, username);
        strcpy(t->path, username);

        // 发送用户名
        int err = send(connect_fd, (char*)t, sizeof(*t), 0);
        if (err == -1) {
            error(0, errno, "send name error");
            continue; // 发送失败，重新输入
        }

        // 接收服务器读取状态
        err = recv(connect_fd, (char*)t, sizeof(*t), 0);
        if (err == -1) {
            error(0, errno, "login_regite1 recv");
            continue; // 接收失败，重新输入
        }

        if (t->cmdType == CMD_TYPE_REGIT1_OK) {
            printf("您输入的用户名没有被注册过\n");
            isUsernameAvailable = 1; // 用户名可用，跳出循环
        } else if (t->cmdType == CMD_TYPE_REGIT1_ERROR) {
            printf("用户名存在，请重新输入\n");
        }
    }

    return 0;
}
int client_regite2(int connect_fd, cmd_tast* t){
    char password[MAX_PASSWORD_LEN];

    // 输入密码
    if (input_client_info("请输入密码: ", password, sizeof(password)) != 0) {
        return -1; // 输入失败
    }

    // 接收服务器发过来的盐值
    int err = recv(connect_fd, (char*)t, sizeof(*t), 0);
    if (err == -1) {
        error(0, errno, "recv the salt error");
        return -1; // 接收盐值失败
    }

    // 将密码和盐值生成密文
    char *encrypted_password = crypt(password, t->path);//t->path存的是盐值
    if (encrypted_password == NULL) {
        perror("crypt failed");
        return -1; // 加密失败
    }
    printf("encrypted_password: %s\n", encrypted_password);

    t->cmdType = CMD_TYPE_REGIT2;
    // 把盐值换成密文
    memset(&t->path, 0, sizeof(t->path));
    strncpy(t->path, encrypted_password, sizeof(t->path));
    //memset(t->content, 0, sizeof(t->content));
    //strncpy(t->content, encrypted_password, sizeof(t->content) - 1);
    t->path[sizeof(t->path) - 1] = '\0';

    // 发送密文到服务器
    err = send(connect_fd, (char *)t, sizeof(*t), 0);
    if (err == -1) {
        error(0, errno, "send cryptpassword error");
        return -1; // 发送密文失败
    }

    // 接收注册结果
    err = recv(connect_fd, (char*)t, sizeof(*t), 0);
    if (err == -1) {
        error(0, errno, "login_regite2 recv error");
        return -1; // 接收注册结果失败
    }

    if (t->cmdType == CMD_TYPE_REGIT_OKK) {
        printf("\n注册成功\n");
        strcpy(t->path, t->content);
        printf("t->content: %s\n", t->content); // 修正打印语句
    }

    // 更新 current_path
    snprintf(current_path, sizeof(current_path), "/%s", t->content);
    

    return 0;
}
