#ifndef __USER_TABLE_H
#define __USER_TABLE_H

#include <stdio.h>

#define USER_TABLE_MAX 40960

typedef struct user_s
{
    int user_fd;
    int user_index;
} user_t;

// 用户表的定义
typedef struct user_table
{
    user_t table[USER_TABLE_MAX];
} user_table_t;

struct user_table *user_table_init();
int user_table_add(int user_fd,int timeoutArrayIndex, user_table_t* ut,int setIdx);
int user_table_erase(int user_fd, user_table_t* ut);
// int user_table_change(int user_fd,int timeoutArrayIndex, user_table_t* ut,int setIdx);
int user_table_find(int user_fd, user_table_t* ut,int *setIdx);
#endif
