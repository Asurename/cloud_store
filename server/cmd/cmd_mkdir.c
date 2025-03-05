#include "server_cmd.h"
// int cmd_mkdir(cmd_tast* t,MYSQL* p_mysql){
//     strcpy(t->content,"CMD_TYPE_MKDIR");
//     return 0;
// }

typedef struct
{
    int id;
    int parent_id;
    char filename[256];
    int owner_id;
    char hash[256];
    int filesize;
    char type[50];
    char file_path[512];
} FileRecord;

int split_string_to_array(const char *input, char buf[][512], char buf_cpy[][512]);
FileRecord *get_info(cmd_tast *t, MYSQL *p_mysql);

int cmd_mkdir(cmd_tast *t, MYSQL *p_mysql)
{
    // 调用函数读取数据
    FileRecord *records = get_info(t, p_mysql);

    char buf[512][512];
    bzero(buf, sizeof(buf));

    char buf_cpy[512][512];
    bzero(buf_cpy, sizeof(buf_cpy));

    char buf_content[512];
    bzero(buf_content, sizeof(buf_content));

    strcat(buf_content, "mkdir: Unable to create directory. ");
    int is_exist = 0;

    for (size_t i = 0; i < 512; i++)
    {
        strcat(buf[i], t->path);
        strcat(buf[i], "/");
        /* code */
    }

    int count = split_string_to_array(t->content, buf, buf_cpy);

    for (size_t i = 1; i < count; i++)
    {
        char query[512];
        bzero(query, sizeof(query));

        snprintf(query, sizeof(query),
                 "SELECT * FROM virtual_file_table WHERE file_path = '%s'", buf[i]);

        // 执行查询
        if (mysql_query(p_mysql, query))
        {
            fprintf(stderr, "Query failed: %s\n", mysql_error(p_mysql));
            mysql_close(p_mysql);
            return 1;
        }

        // 获取结果集
        MYSQL_RES *result = mysql_store_result(p_mysql);
        if (result == NULL)
        {
            fprintf(stderr, "Error: %s\n", mysql_error(p_mysql));
            mysql_close(p_mysql);
            return 1;
        }

        // 判断结果集中是否有匹配的行
        int num_rows = mysql_num_rows(result);
        if (num_rows == 0)
        {

            // 构造插入语句
            char insert_query[512];
            bzero(insert_query, sizeof(insert_query));
            snprintf(insert_query, sizeof(insert_query),
                     "INSERT INTO virtual_file_table (parent_id,filename,owner_id,type,file_path) VALUES ('%d','%s','%d','%d','%s')", records->id, buf_cpy[i], records->owner_id, 1, buf[i]);

            // 执行插入操作
            if (mysql_query(p_mysql, insert_query))
            {
                fprintf(stderr, "Insert failed: %s\n", mysql_error(p_mysql));
                mysql_close(p_mysql);
                return 1;
            }
            continue; // 跳过本次循环，处理下一个路径
        }
        else
        {
            is_exist = 1;
            // mkdir: 无法创建目录 "unit_test/": 文件已存在
            strcat(buf_content, "\"");
            strcat(buf_content, buf_cpy[i]);
            strcat(buf_content, "/\"");
            strcat(buf_content, " ");
        }
    }
    strcat(buf_content, ": The directory already exists.");
    if (is_exist == 1)
    {
        t->Is_printf = 1;
        strcpy(t->content, buf_content);
    }
    else
    {
        bzero(t->content, sizeof(t->content));
    }

    return 0;
}

FileRecord *get_info(cmd_tast *t, MYSQL *p_mysql)
{
    char query[512];
    bzero(query, sizeof(query));

    // snprintf(query, sizeof(query),
    //          "SELECT * FROM virtual_file_table WHERE file_path = '%s'", t->path);
    snprintf(query, sizeof(query),
             "SELECT id, parent_id, filename, owner_id, hash, filesize, type, file_path "
             "FROM virtual_file_table WHERE file_path = '%s'",
             t->path);
    // 执行查询
    if (mysql_query(p_mysql, query))
    {
        fprintf(stderr, "Query failed: %s\n", mysql_error(p_mysql));
        mysql_close(p_mysql);
        exit(EXIT_FAILURE);
    }

    // 获取结果集
    MYSQL_RES *result = mysql_store_result(p_mysql);
    if (result == NULL)
    {
        fprintf(stderr, "Error: %s\n", mysql_error(p_mysql));
        mysql_close(p_mysql);
        exit(EXIT_FAILURE);
    }

    // 获取行数
    int num_rows = mysql_num_rows(result);

    // 动态分配内存存储记录
    FileRecord *records = (FileRecord *)malloc(num_rows * sizeof(FileRecord));
    if (records == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        mysql_free_result(result);
        return NULL;
    }

    // 遍历结果集并将数据存储到结构体数组中
    MYSQL_ROW row;
    int index = 0;

    while ((row = mysql_fetch_row(result)))
    {
        // id 字段
        records[index].id = row[0] ? atoi(row[0]) : -1; // 如果为 NULL，设置为 -1

        // parent_id 字段
        records[index].parent_id = row[1] ? atoi(row[1]) : -1;

        // filename 字段
        if (row[2])
        {
            strncpy(records[index].filename, row[2], sizeof(records[index].filename) - 1);
        }
        else
        {
            strncpy(records[index].filename, "NULL", sizeof(records[index].filename) - 1);
        }
        records[index].filename[sizeof(records[index].filename) - 1] = '\0'; // 确保以 '\0' 结尾

        // owner_id 字段
        records[index].owner_id = row[3] ? atoi(row[3]) : -1;

        // hash 字段
        if (row[4])
        {
            strncpy(records[index].hash, row[4], sizeof(records[index].hash) - 1);
        }
        else
        {
            strncpy(records[index].hash, "NULL", sizeof(records[index].hash) - 1);
        }
        records[index].hash[sizeof(records[index].hash) - 1] = '\0';

        // filesize 字段
        records[index].filesize = row[5] ? atoi(row[5]) : -1;

        // type 字段
        if (row[6])
        {
            strncpy(records[index].type, row[6], sizeof(records[index].type) - 1);
        }
        else
        {
            strncpy(records[index].type, "NULL", sizeof(records[index].type) - 1);
        }
        records[index].type[sizeof(records[index].type) - 1] = '\0';

        // file_path 字段
        if (row[7])
        {
            strncpy(records[index].file_path, row[7], sizeof(records[index].file_path) - 1);
        }
        else
        {
            strncpy(records[index].file_path, "NULL", sizeof(records[index].file_path) - 1);
        }
        records[index].file_path[sizeof(records[index].file_path) - 1] = '\0';

        index++;
    }
    return records;
}

int split_string_to_array(const char *input, char buf[][512], char buf_cpy[][512])
{
    char temp_input[4096];     // 临时缓冲区，避免修改原始输入
    strcpy(temp_input, input); // 复制输入字符串到临时缓冲区

    int word_count = 0; // 记录单词数量

    // 使用 strtok 分割字符串
    char *token = strtok(temp_input, " "); // 第一次调用，指定分隔符为空格
    while (token != NULL && word_count < 512)
    {
        if (strlen(token) > 0)
        {
            strcat(buf[word_count], token);     // 确保 token 不是空字符串
            strcat(buf_cpy[word_count], token); // 确保 token 不是空字符串
            // strncpy(buf[word_count], token, 512 - 1); // 安全复制到 buf
            buf[word_count][512 - 1] = '\0'; // 确保字符串以 '\0' 结尾
            word_count++;
        }
        token = strtok(NULL, " "); // 继续分割剩余部分
    }

    return word_count; // 返回单词数量
}