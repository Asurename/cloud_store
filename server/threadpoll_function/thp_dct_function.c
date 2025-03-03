#include "../lib.h"

pthread_mutex_t timeout_index_mutex = PTHREAD_MUTEX_INITIALIZER;
int timeoutArrayIndex = 0;

int timeout_array_add(int (*netfdArray)[1024], user_table_t *userTable, int timeoutArrayIndex, int user_fd)
{
    printf("Thread %ld: Adding user_fd %d to timeoutArrayIndex %d\n", pthread_self(), user_fd, timeoutArrayIndex);
    if (timeoutArrayIndex == 0)
    {
        for (int i = 0; i < TIMEOUT_MAX; i++)
        {
            if (netfdArray[TIMEOUT_ARRAY - 1][i] == -1)
            {
                netfdArray[TIMEOUT_ARRAY - 1][i] = user_fd;
                user_table_add(user_fd, timeoutArrayIndex, userTable);
                printf("Thread %ld: Added user_fd %d at netfdArray[%d][%d]\n", pthread_self(), user_fd, TIMEOUT_ARRAY - 1, i);
                break;
            }
        }
    }
    else
    {
        for (int i = 0; i < TIMEOUT_MAX; i++)
        {
            if (netfdArray[timeoutArrayIndex - 1][i] == -1)
            {
                netfdArray[timeoutArrayIndex - 1][i] = user_fd;
                user_table_add(user_fd, timeoutArrayIndex, userTable);
                printf("Thread %ld: Added user_fd %d at netfdArray[%d][%d]\n", pthread_self(), user_fd, timeoutArrayIndex - 1, i);
                break;
            }
        }
    }
    return 0;
}

int timeout_array_delete(int (*netfdArray)[1024], user_table_t *userTable, int timeoutArrayIndex)
{
    if (timeoutArrayIndex < 0 || timeoutArrayIndex >= TIMEOUT_ARRAY) {
        fprintf(stderr, "Invalid timeoutArrayIndex: %d\n", timeoutArrayIndex);
        return -1;
    }
    //printf("Thread %ld: Deleting users from timeoutArrayIndex %d\n", pthread_self(), timeoutArrayIndex);
    for (int i = 0; i < TIMEOUT_MAX; i++)
    {
        if (netfdArray[timeoutArrayIndex][i] != -1)
        {
            close(netfdArray[timeoutArrayIndex][i]);
            user_table_erase(netfdArray[timeoutArrayIndex][i], userTable);
            netfdArray[timeoutArrayIndex][i] = -1;
            printf("Thread %ld: Deleted user_fd from netfdArray[%d][%d]\n", pthread_self(),timeoutArrayIndex, i);
        }
    }
    return 0;
}

int timeout_array_change(int (*netfdArray)[1024], user_table_t *userTable,int timeoutArrayIndex,int user_fd)
{
    printf("Thread %ld: Changing user_fd %d to timeoutArrayIndex %d\n", pthread_self(), user_fd, timeoutArrayIndex);
    int idx = user_table_find(user_fd, userTable);
    printf("------------------index : %d\n",idx);
    if (idx == -1 || idx < 0 || idx >= TIMEOUT_ARRAY) {
        fprintf(stderr, "Invalid user_fd %d or index %d\n", user_fd, idx);
        return -1;
    }

    for (int i = 0; i < TIMEOUT_MAX; i++)
    {
        if (netfdArray[idx][i] == user_fd)
        {
            timeout_array_add(netfdArray,userTable,timeoutArrayIndex,user_fd);
            user_table_change(user_fd,timeoutArrayIndex,userTable);
            netfdArray[idx][i] = -1;
            printf("Thread %ld: Moved user_fd %d from netfdArray[%d][%d] to netfdArray[%d][new_index]\n", pthread_self(), user_fd, idx, i, timeoutArrayIndex -1);
            break;
        }
    }
    return 0;
}

// 自动断开连接线程的执行函数
void *thp_dct_function(void *arg)
{
    user_table_t *userTable = (user_table_t *)arg;

    memset(netfdArray, -1, sizeof(netfdArray));
    while (1)
    {
        sleep(1);
        pthread_mutex_lock(&timeout_index_mutex);
        printf("Thread %ld: Processing timeoutArrayIndex %d\n", pthread_self(), timeoutArrayIndex);
        timeoutArrayIndex = (timeoutArrayIndex + 1) % TIMEOUT_ARRAY;
        int current_index = timeoutArrayIndex;
        pthread_mutex_unlock(&timeout_index_mutex);
        timeout_array_delete(netfdArray,userTable,current_index);
    }
    return NULL;
}
