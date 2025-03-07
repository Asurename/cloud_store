#include "server_cmd.h"
#include "../tool/string_stack.h"
// int cmd_cd(cmd_tast* t,MYSQL* p_mysql){
//     strcpy(t->content,"CMD_TYPE_CD");
//     return 0;
// }

void strtok_commend(char *commend, StringStack *stack)
{
    const char delim[] = " /";
    char *token = strtok(commend, delim);
    while (token != NULL)
    {
        token = strtok(NULL, delim);
        if (token == NULL)
        {
            break;
        }
        if (strcmp(token, "..") == 0)
        {
            if (is_string_stack_empty(stack))
            {

                continue;
            }
            pop_string(stack);
        }
        else if (strcmp(token, ".") == 0)
        {
            continue;
        }
        else
        {
            push_string(stack, token);
        }
    }
}
void strtok_path(char *path, StringStack *stack)
{
    const char delim[] = "/\n";
    char *token = strtok(path, delim);
    while (token != NULL)
    {
        if (token == NULL)
        {
            break;
        }
        push_string(stack, token);
        printf("%s\n", token);
        token = strtok(NULL, delim);
    }
}

int cmd_cd(cmd_tast *t, MYSQL *p_mysql)
{
    char buf[4096];
    char path_temp[512];
    strcpy(path_temp, t->path);

    StringStack stack;
    init_string_stack(&stack);
    strtok_path(t->path, &stack);
    strtok_commend(t->content, &stack);

    bzero(buf, sizeof(buf));
    for (int i = 0; i <= stack.top; ++i)
    {
        strcat(buf, "/");
        strcat(buf, stack.data[i]);
    }
    printf("%s\n", buf);

    char query[512];
    // 构造查询字符串
    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM virtual_file_table WHERE file_path = '%s' AND type = 1", buf);

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
        printf("count: %d\n", count);
        if (count == 1)
        {
            t->Is_printf = 0;
            bzero(t->content, sizeof(t->content));
            bzero(t->path, sizeof(t->path));
            strcpy(t->path, buf);
        }
        else
        {
            t->Is_printf = 1;
            bzero(t->content, sizeof(t->content));
            if (is_string_stack_empty(&stack) == 1)
            {
                strcpy(t->content, "You don't have permissions.");
            }
            else
            {
                strcpy(t->content, "There is no such file or directory.");
            }
            bzero(t->path, sizeof(t->path));
            strcpy(t->path, path_temp);
        }
    }

    return 0;
}
