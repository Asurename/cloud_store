#ifndef __MYSQL_IO_H
#define __MYSQL_IO_H
#include "../lib.h"

typedef struct{
    const char* parent_id;//指向文件父目录的标识
    const char* owner_id;//指向文件所有者的标识
    const char* filename;//指向文件名的指针
    const char* filesize;//指向文件大小的指针
    const char* file_path;//指向文件路径的指针
    const char* hash;//指向文件哈希值的指针
    const char* type;//指向文件类型的标识
}file_data;

typedef struct{
    const char* username;//指向用户名的指针
    const char* salt;//指向盐值
    const char* cryptpasswd;//指向密文
    const char* pwd;
}user_data;

MYSQL* mysql_disk_connect(const char* host, const short port, const char* username, const char* passwd, const char* database);
//判断用户名是否存在、
int mysql_check_username(MYSQL* p_mysql, const char* username);
//写入用户名和盐值
int mysql_write_user_data1(MYSQL* p_mysql, user_data user_input_data);
//写入用户的密文
int mysql_write_user_data2(MYSQL* p_mysql, user_data user_input_data);
//虚拟文件表写入
int mysql_write_file_data(MYSQL* p_mysql,file_data file_input_data);
//读取用户id
int mysql_get_user_id(MYSQL* p_mysql, char* buf, const char* username);
//用户表读取
int mysql_get_user_data(MYSQL* p_mysql, char* buf, const char* username, const char* field);
int mysql_get_user_path(MYSQL* p_mysql, char* buf, const char* cryptpasswd, const char* field);
int mysql_get_user_info(MYSQL* p_mysql, char* buf, const char* condition_field, const char* condition_value, const char* target_field);
int mysql_get_file_data(MYSQL* p_mysql, char* buf, const char* key_name, const char* key_value, const char* field);
#endif
