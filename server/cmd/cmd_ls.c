#include "server_cmd.h"
#define ANSI_COLOR_GREEN "\x1b[95m"
#define ANSI_COLOR_BRIGHT_MAGENTA "\x1b[95m"
#define ANSI_COLOR_RESET "\x1b[0m"

int retain_or_not(char *path, int curr_count);
int count_path(char *path);
int cmd_ls(cmd_tast *t, MYSQL *p_mysql)
{
    t->Is_printf = 1;
    bzero(&t->content, sizeof(t->content));
    char buf[4096];
    bzero(buf, sizeof(buf));
    strcpy(buf, t->path);

    int count_curr_path = count_path(buf);
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT * FROM virtual_file_table WHERE file_path LIKE '%s%%'", t->path);

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

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)))
    {
        strcpy(buf, row[7]);
        if (retain_or_not(buf, count_curr_path) == 0)
        {
            continue;
        }
        if (strcmp(row[6], "1") == 0)
        {
            printf("type === %s\n", row[6]);
            strcat(t->content, ANSI_COLOR_BRIGHT_MAGENTA);
            strcat(t->content, row[2]);
            strcat(t->content, ANSI_COLOR_RESET);
            strcat(t->content, "   ");
        }
        else if (strcmp(row[6], "2") == 0)
        {
            /* code */
            strcat(t->content, row[2]);
            strcat(t->content, "   ");
        }

        continue;
        // printf("\n");
    }

    return 0;
}

int count_path(char *path)
{
    int count = 0;
    const char delim[] = "/";
    char *token = strtok(path, delim);
    while (token != NULL)
    {
        ++count;
        token = strtok(NULL, delim);
    }
    return count;
}

int retain_or_not(char *path, int curr_count)
{
    const char delim[] = "/";
    char *token = strtok(path, delim);
    int count = 0;
    while (token != NULL)
    {
        ++count;
        token = strtok(NULL, delim);
    }
    return count - curr_count == 1 ? 1 : 0;
}
