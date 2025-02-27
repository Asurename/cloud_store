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
