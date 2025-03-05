#include "register.h"
#include "../mysql/mysql_io.h"
#include <stdlib.h>
#include <string.h>

// 生成随机字符串
char* generate_random_string(int length) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";
    char* random_string = (char*)malloc((length + 1) * sizeof(char));
    if (random_string == NULL) {
        return NULL;
    }

    srand(time(NULL));
    for (int i = 0; i < length; i++) {
        int index = rand() % (sizeof(charset) - 1);
        random_string[i] = charset[index];
    }
    random_string[length] = '\0';
    return random_string;
}

// 生成盐值
char* crypt_gensalt(const char* prefix, int rounds, const char* salt_str, int extra_flags) {
    // 盐值长度通常为8或16，这里选择8
    const int salt_length = 8;

    // 如果传入的盐值为空，则生成一个随机盐值
    char* salt;
    if (salt_str == NULL) {
        salt = generate_random_string(salt_length);
        if (salt == NULL) {
            return NULL;
        }
    } else {
        salt = strdup(salt_str);
        if (salt == NULL) {
            return NULL;
        }
    }

    // 构建完整的盐值字符串
    char* full_salt = (char*)malloc((strlen(prefix) + 1 + salt_length + 1) * sizeof(char));
    if (full_salt == NULL) {
        free(salt);
        return NULL;
    }
    sprintf(full_salt, "%s%d$%s", prefix, rounds, salt);

    free(salt);
    return full_salt;
}

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
//处理注册的第一阶段，检查用户名是否存在，若不存在则生成盐值并将用户名、盐值等信息存入数据库，同时将盐值发送给客户端。
int server_regite1(cmd_tast* t,MYSQL* p_mysql){
    printf("t->content:%s\n",t->content); //这里t->content是用户名

    // 查询用户名是否存在
    int err = mysql_check_username(p_mysql, (const char*)t->content);
    if (err == -1) {
    error(0, errno, "mysql_check_username error");
    return -1;
}
    if (err == 1) {
    // 用户名存在，设置为注册错误类型
    t->cmdType = CMD_TYPE_REGIT1_ERROR;
    printf("用户名已存在\n");
    return 0;
}
    else {
    // 用户名不存在，设置为注册成功类型并生成盐值
    printf("'%s'正在输入密码\n", t->content);
    t->cmdType = CMD_TYPE_REGIT1_OK;
    err = send(t->peerfd, (char*)t, sizeof(*t), 0);
    if (err == -1) {
        error(0, errno, "send name error");
        return -1;
    }

    // 生成盐值
    const char* prefix = "$6$";
    char* salt = crypt_gensalt(prefix, 5, NULL, 0);
    if (salt == NULL) {
        error(0, errno, "crypt_gensalt failed");
        return -1;
    }
    // 访问数据库存储用户名和盐值
    user_data inputData = {
       .username = t->content,
       .salt = salt,
       .cryptpasswd = "",  // 初始化为空，等待客户端发送真实密码
       .pwd = t->path
    };

    printf("t->content:%s\n",t->content);
    err = mysql_write_user_data1(p_mysql, inputData);
    if (err == -1) {
        error(0, errno, "mysql_write_user_data1 failed");
        free(salt);  // 释放盐值内存
        return -1;
    }

    // 填充盐值到 path 字段用来传递盐值
    memset(t->path, 0, sizeof(t->path));
    snprintf(t->path, sizeof(t->path), "%s", salt);

    free(salt);  // 释放盐值内存
}
    return 0;
}
// 处理注册的第二阶段，接收客户端发送的加密密码并存储到数据库，然后初始化用户的虚拟文件表。
int server_regite2(cmd_tast* t,MYSQL* p_mysql){
    if (t->cmdType == CMD_TYPE_REGIT2) {
        // 接收用户发过来的密文 path 字段，存入数据库
        user_data inputData = {
           .cryptpasswd = t->path,// 密码
           .username = t->content // 用户名
        };
        int err = mysql_write_user_data2(p_mysql, inputData);
        if (err == -1) {
            error(0, errno, "mysql_write_user_data2 error");
            return -1;
        }
    
        // 注册成功状态
        t->cmdType = CMD_TYPE_REGIT_OKK;
        printf("用户'%s'注册成功\n", t->content);
    }
    // 得到用户 id
    char buf[128]={0};
    int err = mysql_get_user_id(p_mysql, buf, t->content);
    if (err == -1) {
        error(0, errno, "mysql_get_user_id failed");
        return -1;
    }
    printf("buf:%s\n",buf);

    char file_path_data[4096]={0};
    snprintf(file_path_data, sizeof(file_path_data), "/%s", t->content);

    // 初始化虚拟文件表
    file_data initialize_file = {
       .parent_id = "0",
       .filename = t->content,
       .owner_id = buf,
       .hash = "",
       .filesize = "0",
       .type = "1",
       .file_path = file_path_data
    };
    // 添加斜杠和用户名到 file_path
    //snprintf(initialize_file.file_path, sizeof(initialize_file.file_path), "/%s", t->content);

    //strcpy(initialize_file.owner_id, buf);  // 复制用户 ID
    printf("initialize_file.owner_id:%s\n",initialize_file.owner_id);
    err = mysql_write_file_data(p_mysql, initialize_file);
    if (err == -1) {
        error(0, errno, "mysql_write_file_data failed");
        return -1;
    }
    return 0;
}
