#ifndef __USER_TABLE_H
#define __USER_TABLE_H

#include <stdio.h>

//用户表的定义
struct user_table{
    int a;    
};

struct user_table* user_table_init();
int user_table_add(int user_fd,struct user_table *ut);
int user_table_erase(int user_fd,struct user_table *ut);
#endif
