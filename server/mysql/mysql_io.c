#include "mysql_io.h"

MYSQL* mysql_disk_connect(const char* host, const short port, const char* username, const char* passwd, const char* database) {
    MYSQL* p_mysql = mysql_init(NULL);
    p_mysql = mysql_real_connect(p_mysql, host, username, passwd, database, port, NULL, 0);
    if (p_mysql == NULL) {
        fprintf(stderr, "mysql connect failed\n");
        return NULL;
    }
    printf("[Threadpool]mysql connect success\n");
    return p_mysql;
}

//判断用户名是否存在、
int mysql_check_username(MYSQL* p_mysql, const char* username){
    char query[256];
    sprintf(query, "SELECT COUNT(*) FROM user_data WHERE username = '%s'", username);


    int ret = mysql_query(p_mysql, query);
    if (ret != 0) {
        error(0, 0, "mysql_query failed: %s", mysql_error(p_mysql));
        return -1;  // 查询失败
    }

    MYSQL_RES* res = mysql_store_result(p_mysql);
    if (res == NULL) {
        error(0, 0, "mysql_store_result failed: %s", mysql_error(p_mysql));
        return -1;  // 结果存储失败
    }

    MYSQL_ROW row = mysql_fetch_row(res);
    int exists = (row != NULL && atoi(row[0]) > 0);  // 如果返回行数 > 0，则用户名存在


    mysql_free_result(res);

    return exists ? 1 : 0;  // 返回 1 表示存在，0 表示不存在
}
//写入用户名和盐值
int mysql_write_user_data1(MYSQL* p_mysql, user_data user_input_data){
    char query[256];
    // 打印写入的数据
    sprintf(query, "INSERT INTO user_data (username, salt,cryptpasswd, pwd) VALUES ('%s','%s', '%s','%s')",
        user_input_data.username,
        user_input_data.salt,
        user_input_data.cryptpasswd,
        user_input_data.pwd);

    printf("query:%s \n length:%d",query,strlen(query));
    int ret = mysql_query(p_mysql, query);
    if (ret != 0) {
        error(0, 0, "mysql_query");
    }

    return 0;
}
//写入用户的密文
int mysql_write_user_data2(MYSQL* p_mysql, user_data user_input_data){
    char query[256];
    sprintf(query, "UPDATE user_data "
        "SET cryptpasswd = '%s' "
        "WHERE username = '%s'",
        user_input_data.cryptpasswd, user_input_data.username);
    printf("query:%s \nlength:%d",query,strlen(query));
    int ret = mysql_query(p_mysql, query);
    if (ret != 0) {
        //error(0, 0, "mysql_query");
        mysql_error(p_mysql);
    }
    return 0;
}
//虚拟文件表写入
int mysql_write_file_data(MYSQL* p_mysql,file_data file_input_data){
    char query[256];
    sprintf(query, "insert into virtual_file_table \ 
            (parent_id,filename,owner_id,hash,filesize,type,file_path) \
            values(%s,'%s',%s,'%s',%s,%s,'%s')",
            file_input_data.parent_id,
            file_input_data.filename,
            file_input_data.owner_id,
            file_input_data.hash,
            file_input_data.filesize,
            file_input_data.type,
            file_input_data.file_path);
    int ret = mysql_query(p_mysql, query);
    if (ret != 0) {
        error(0, 0, "mysql_query");
        return -1;
    }

    return 0;
}
//读取用户id
int mysql_get_user_id(MYSQL* p_mysql, char* buf, const char* username){
    char query[256];
    sprintf(query, "select id from user_data where username = '%s'", username);
    int ret = mysql_query(p_mysql, query);
    if (ret != 0) {
        error(0, 0, "mysql_query");
        return -1;
    }
    MYSQL_ROW row;
    MYSQL_RES* res = mysql_use_result(p_mysql);
    row = mysql_fetch_row(res);
    if (row == NULL) {
        printf("Query fail\n\n");
        return -1;
    }

    printf("Query user id: %s\n\n", row[0]);
    strcpy(buf, row[0]);
    mysql_free_result(res);
    return 0;
}
//查表获取"username"
int mysql_get_user_data(MYSQL* p_mysql, char* buf, const char* username, const char* field) {
    char query[256];
    sprintf(query, "select %s from user_data where username = '%s'", field, username);
    int ret = mysql_query(p_mysql, query);
    if (ret != 0) {
        error(0, 0, "mysql_query");
        return -1;
    }
    MYSQL_ROW row;
    MYSQL_RES* res = mysql_use_result(p_mysql);
    row = mysql_fetch_row(res);
        if(row==NULL)
    {
        printf("Query failure\n\n");
        return -1;
    }
    strcpy(buf, row[0]);
    mysql_free_result(res);
    return 0;
}
//查表获取"cryptpasswd"
int mysql_get_user_path(MYSQL* p_mysql, char* buf, const char* cryptpasswd, const char* field){
    char query[256];
    sprintf(query, "select %s from user_data where cryptpasswd = '%s'", field, cryptpasswd);
    int ret = mysql_query(p_mysql, query);
    if (ret != 0) {
        error(0, 0, "mysql_query");
        return -1;
    }
    MYSQL_ROW row;
    MYSQL_RES* res = mysql_use_result(p_mysql);
    row = mysql_fetch_row(res);
    if(row==NULL)
    {
        printf("Query fail\n\n");
        return -1;
    }

    printf("Query user path: %s\n\n",row[0]);
    strcpy(buf, row[0]);
    mysql_free_result(res);
    return 0;
}
//通用的查询函数 login1查的是盐 login2查的是pwd


//通用的查询函数
int mysql_get_user_info(MYSQL* p_mysql, char* buf,
                        const char* condition_field,
                        const char* condition_value,
                        const char* target_field) {
    char query[256];
    sprintf(query, "select %s from user_data where %s = '%s'", target_field, condition_field, condition_value);
    int ret = mysql_query(p_mysql, query);
    if (ret != 0) {
        error(0, 0, "mysql_query");
        return -1;
    }
    MYSQL_ROW row;
    MYSQL_RES* res = mysql_use_result(p_mysql);
    row = mysql_fetch_row(res);
    if(row == NULL) {
        printf("Query fail\n\n");
        return -1;
    }
    if (strcmp(condition_field, "cryptpasswd") == 0) {
        printf("Query user path: %s\n\n", row[0]);
    }
    strcpy(buf, row[0]);
    mysql_free_result(res);
    return 0;
}

int mysql_get_file_data(MYSQL* p_mysql, char* buf,
                        const char* key_name,
                        const char* key_value,
                        const char* field) {
    char query[256];
    if(strcmp(key_name,"filename")== 0 || strcmp(key_name,"file_path") == 0||strcmp(key_name,"hash") == 0){
        sprintf(query, "select %s from virtual_file_table where %s = '%s'",
                field,
                key_name,
                key_value);
    }else{
        sprintf(query, "select %s from virtual_file_table where %s = %s",
                field,
                key_name,
                key_value);
    }
    int ret = mysql_query(p_mysql, query);
    if (ret != 0) {
        error(0, 0, "mysql_query");
        return -1;
    }
    MYSQL_ROW row;
    MYSQL_RES* res = mysql_use_result(p_mysql);
    row = mysql_fetch_row(res);
    strcpy(buf, row[0]);
    mysql_free_result(res);
    return 0;
}
