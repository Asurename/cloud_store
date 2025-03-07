#include "server_cmd.h"
// int cmd_rm(cmd_tast *t, MYSQL *p_mysql)
// {
//     strcpy(t->content, "CMD_TYPE_RM");
//     return 0;
// }

char **splitStringBySpace1(const char *input, int *size);

int cmd_rm(cmd_tast *t, MYSQL *p_mysql)
{
    t->Is_printf = 0;
    printf("curr_path = %s\n", t->path);
    int size = 0;
    // 调用函数分割字符串
    char **result = splitStringBySpace1(t->content, &size);

    bzero(t->content, sizeof(t->content));

    char buf[size][512];
    bzero(buf, sizeof(buf));
    // 打印结果
    printf("Split result (%d parts):\n", size);
    for (int i = 0; i < size; i++)
    {
        strcat(buf[i], t->path);
        strcat(buf[i], "/");
        strcat(buf[i], result[i]);

        printf("Part %d: %s\n", i + 1, result[i]);
        printf("Part %d: %s\n", i + 1, buf[i]);
    }

    int no_match_found_count = 0;
    int is_no_match_found = 0;
    char no_match_found[20][128];
    bzero(no_match_found, sizeof(no_match_found));

    int non_empty_count = 0;
    int is_non_empty = 0;
    char non_empty[20][128];
    bzero(non_empty, sizeof(non_empty));

    for (size_t i = 0; i < size; i++)
    {
        char dir_name[512];
        strcpy(dir_name, result[i]);
        printf("dir_name = %s\n", dir_name);
        printf("=============\n");
        printf("path = %s\n", buf[i]);
        char query[512];
        bzero(query, sizeof(query));

        snprintf(query, sizeof(query),
                 "SELECT count(*) FROM virtual_file_table WHERE file_path = '%s'", buf[i]);

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

        MYSQL_ROW row = mysql_fetch_row(result);
        if (row != NULL)
        {
            int count = atoi(row[0]);
            if (count == 1)
            {
                char query1[512];
                bzero(query1, sizeof(query1));
                snprintf(query1, sizeof(query1),
                         "SELECT count(*) FROM virtual_file_table WHERE file_path = '%s' AND type = 2", buf[i]);
                mysql_query(p_mysql, query1);
                MYSQL_RES *result1 = mysql_store_result(p_mysql);
                MYSQL_ROW row1 = mysql_fetch_row(result1);
                int count1 = atoi(row1[0]);

                printf("count1: %d\n", count1);
                if (count1 == 1)
                {
                    snprintf(query, sizeof(query),
                             "DELETE FROM virtual_file_table WHERE file_path = '%s'", buf[i]);
                    // 执行查询
                    if (mysql_query(p_mysql, query))
                    {
                        fprintf(stderr, "Delete failed: %s\n", mysql_error(p_mysql));
                        return -1; // 返回 -1 表示错误
                    }
                }
                else
                {
                    t->Is_printf = 1;
                    strcpy(no_match_found[no_match_found_count], dir_name);
                    no_match_found_count++;
                }

                // // 获取受影响的行数
                // int affectedRows = mysql_affected_rows(p_mysql);
                // if (affectedRows == 0)
                // {
                // printf("affectedRows = %d\n", affectedRows);

                // printf("No rows deleted (no match found for file_path = '%s').\n", buf[i]);
                // }
            }
            else
            {
                printf("@@@\n");
                t->Is_printf = 1;
                strcpy(non_empty[non_empty_count], dir_name);
                non_empty_count++;
            }

            printf("count: %d\n", count);
        }
    }

    printf("=============\n");
    printf("no_match_found_count = %d\n", no_match_found_count);
    printf("non_empty_count = %d\n", non_empty_count);

    if (no_match_found_count != 0)
    {
        printf("wow\n");
        strcat(t->content, "rm: can't delete ");
        for (size_t i = 0; i < no_match_found_count; i++)
        {
            strcat(t->content, "'");
            strcat(t->content, no_match_found[i]);
            strcat(t->content, "' ");
            /* code */
        }

        strcat(t->content, "failed: This is directory .\n");
    }
    // printf("%s\n", t->content);
    if (non_empty_count != 0)
    {
        strcat(t->content, "rm: can't delete ");
        for (size_t i = 0; i < non_empty_count; i++)
        {
            strcat(t->content, "'");
            strcat(t->content, non_empty[i]);
            strcat(t->content, "' ");
        }

        strcat(t->content, "failed: There is no file.");
    }

    for (int i = 0; i < size; i++)
    {
        free(result[i]);
    }
    free(result);
    // strcpy(t->content, "CMD_TYPE_RMDIR");
    return 0;
};

// 函数定义
char **splitStringBySpace1(const char *input, int *size)
{
    // 找到第一个非空格字符的位置
    while (*input == ' ')
    {
        input++;
    }

    // 创建一个可修改的副本，因为 strtok 会修改原字符串
    char *strCopy = strdup(input);
    if (strCopy == NULL)
    {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    // 计算分割后的字符串数量
    int capacity = 10; // 初始容量
    char **result = malloc(capacity * sizeof(char *));
    if (result == NULL)
    {
        fprintf(stderr, "Memory allocation failed.\n");
        free(strCopy);
        exit(EXIT_FAILURE);
    }

    // 使用 strtok 分割字符串
    char *token = strtok(strCopy, " ");
    *size = 0;

    // 跳过第一个单词
    if (token != NULL)
    {
        token = strtok(NULL, " "); // 跳过第一个单词
    }

    while (token != NULL)
    {
        // 去掉多余的空格（多个空格算一个）
        if (strlen(token) > 0)
        {
            // 如果当前容量不足，则扩容
            if (*size >= capacity)
            {
                capacity *= 2;
                result = realloc(result, capacity * sizeof(char *));
                if (result == NULL)
                {
                    fprintf(stderr, "Memory reallocation failed.\n");
                    free(strCopy);
                    for (int i = 0; i < *size; i++)
                    {
                        free(result[i]);
                    }
                    free(result);
                    exit(EXIT_FAILURE);
                }
            }

            // 分配内存并复制当前 token
            result[*size] = strdup(token);
            if (result[*size] == NULL)
            {
                fprintf(stderr, "Memory allocation failed.\n");
                free(strCopy);
                for (int i = 0; i < *size; i++)
                {
                    free(result[i]);
                }
                free(result);
                exit(EXIT_FAILURE);
            }

            (*size)++;
        }

        // 获取下一个 token
        token = strtok(NULL, " ");
    }

    // 释放临时副本
    free(strCopy);

    return result;
}