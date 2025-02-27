#ifndef __MYSQL_IO_H
#define __MYSQL_IO_H
#include "../lib.h"

MYSQL* mysql_disk_connect(const char* host, const short port, const char* username, const char* passwd, const char* database);
#endif
