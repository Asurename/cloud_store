#ifndef __THP_DCT_FUNCTION_H
#define __THP_DCT_FUNCTION_H

#define TIMEOUT_MAX 1024
#define TIMEOUT_ARRAY 30

extern pthread_mutex_t timeout_index_mutex;
int netfdArray[TIMEOUT_ARRAY][TIMEOUT_MAX];
extern int  timeoutArrayIndex;

void* thp_dct_function(void* arg);
int timeout_array_add(int (*netfdArray)[1024], user_table_t *userTable, int timeoutArrayIndex, int user_fd);
int timeout_array_delete(int (*netfdArray)[1024], user_table_t *userTable, int timeoutArrayIndex);
int timeout_array_change(int (*netfdArray)[1024], user_table_t *userTable,int timeoutArrayIndex,int user_fd);

#endif
