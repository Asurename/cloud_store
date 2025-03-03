#include "../lib.h"
// 初始化用户表
user_table_t* user_table_init() {
    user_table_t* ut = (user_table_t*)malloc(sizeof(user_table_t));
    if (ut == NULL) {
        perror("Failed to initialize user table");
        return NULL;
    }
    for (int i = 0; i < USER_TABLE_MAX; i++) {
        ut->table[i].user_fd = -1;
        ut->table[i].user_index = -1;
    }
    return ut;
}

// 向用户表中添加用户
int user_table_add(int user_fd,int timeoutArrayIndex, user_table_t* ut) {
    if (ut == NULL) {
        fprintf(stderr, "User table is not initialized\n");
        return -1;
    }
    for (int i = 0; i < USER_TABLE_MAX; i++) {
        if (ut->table[i].user_fd == -1) { 
            ut->table[i].user_fd = user_fd;
            ut->table[i].user_index = timeoutArrayIndex -1;
            return timeoutArrayIndex -1; 
        }
    }
    fprintf(stderr, "User table is full\n");
    return -1; 
}

// 从用户表中删除用户
int user_table_erase(int user_fd,user_table_t* ut) {
    if (ut == NULL) {
        fprintf(stderr, "User table is not initialized\n");
        return -1;
    }
    for (int i = 0; i < USER_TABLE_MAX; i++) {
        if (ut->table[i].user_fd == user_fd) {
            ut->table[i].user_fd = -1;
            ut->table[i].user_index = -1;
            return 0;
        }
    }
    fprintf(stderr, "user_table_erase:User not found\n");
    return -1;
}

// 修改用户表中的用户信息
int user_table_change(int user_fd,int timeoutArrayIndex, user_table_t* ut) {
    if (ut == NULL) {
        fprintf(stderr, "User table is not initialized\n");
        return -1;
    }
    for (int i = 0; i < USER_TABLE_MAX; i++) {
        if (ut->table[i].user_fd == user_fd && timeoutArrayIndex != 0) { 
            ut->table[i].user_index = timeoutArrayIndex -1; 
            return 0;
        }

        if (ut->table[i].user_fd == user_fd && timeoutArrayIndex == 0) { 
            ut->table[i].user_index = TIMEOUT_ARRAY - 1; 
            return 0;
        }
    }
    fprintf(stderr, "user_table_change:User not found\n");
    return -1;
}


// 查找用户
int user_table_find(int user_fd, user_table_t* ut) {
    if (ut == NULL) {
        fprintf(stderr, "User table is not initialized\n");
        return -1;
    }
    for (int i = 0; i < USER_TABLE_MAX; i++) {
        if (ut->table[i].user_fd == user_fd) {
            return ut->table[i].user_index;
        }
    }
    fprintf(stderr, "not find %d user_fd\n", user_fd);
    return -1;
}
