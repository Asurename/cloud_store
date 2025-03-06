#include "../lib.h"

pthread_mutex_t timeout_index_mutex = PTHREAD_MUTEX_INITIALIZER;
int timeoutArrayIndex = 0;
int netfdArray[TIMEOUT_ARRAY][TIMEOUT_MAX];

int timeout_array_add(int (*netfdArray)[1024], user_table_t *userTable, int timeoutArrayIndex, int user_fd,int *setIdx)
{
    syslog(LOG_INFO, "Thread %ld: Adding user_fd %d to timeoutArrayIndex %d\n", pthread_self(), user_fd, timeoutArrayIndex);
    printf("Thread %ld: Adding user_fd %d to timeoutArrayIndex %d\n", pthread_self(), user_fd, timeoutArrayIndex);
    if (timeoutArrayIndex == 0)
    {
        for (int i = 0; i < TIMEOUT_MAX; i++)
        {
            if (netfdArray[TIMEOUT_ARRAY - 1][i] == -1)
            {
                setIdx = i;
                netfdArray[TIMEOUT_ARRAY - 1][i] = user_fd;
                user_table_add(user_fd,TIMEOUT_ARRAY - 1, userTable, i);
                syslog(LOG_INFO, "Thread %ld: Added user_fd %d at netfdArray[%d][%d]\n", pthread_self(), user_fd, TIMEOUT_ARRAY - 1, i);
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
                user_table_add(user_fd, timeoutArrayIndex, userTable, i);
                syslog(LOG_INFO, "Thread %ld: Added user_fd %d at netfdArray[%d][%d]\n", pthread_self(), user_fd, timeoutArrayIndex - 1, i);
                printf("Thread %ld: Added user_fd %d at netfdArray[%d][%d]\n", pthread_self(), user_fd, timeoutArrayIndex - 1, i);
                break;
            }
        }
    }
    return 0;
}

int timeout_array_delete(int (*netfdArray)[1024], user_table_t *userTable, int timeoutArrayIndex)
{
    if (timeoutArrayIndex < 0 || timeoutArrayIndex >= TIMEOUT_ARRAY)
    {
        syslog(LOG_ERR, "Invalid timeoutArrayIndex: %d\n", timeoutArrayIndex);
        fprintf(stderr, "Invalid timeoutArrayIndex: %d\n", timeoutArrayIndex);
        return -1;
    }
    // printf("Thread %ld: Deleting users from timeoutArrayIndex %d\n", pthread_self(), timeoutArrayIndex);
    for (int i = 0; i < TIMEOUT_MAX; i++)
    {
        if (netfdArray[timeoutArrayIndex][i] != -1)
        {
            close(netfdArray[timeoutArrayIndex][i]);
            user_table_erase(netfdArray[timeoutArrayIndex][i], userTable);
            netfdArray[timeoutArrayIndex][i] = -1;
            syslog(LOG_INFO, "Thread %ld: Deleted user_fd from netfdArray[%d][%d]\n", pthread_self(), timeoutArrayIndex, i);
            printf("Thread %ld: Deleted user_fd from netfdArray[%d][%d]\n", pthread_self(), timeoutArrayIndex, i);
        }
    }
    return 0;
}

int timeout_array_change(int (*netfdArray)[1024], user_table_t *userTable, int timeoutArrayIndex, int user_fd)
{
    syslog(LOG_INFO, "Thread %ld: Changing user_fd %d to timeoutArrayIndex %d\n", pthread_self(), user_fd, timeoutArrayIndex);
    printf("Thread %ld: Changing user_fd %d to timeoutArrayIndex %d\n", pthread_self(), user_fd, timeoutArrayIndex);
    int setIdx = -1;
    int idx = user_table_find(user_fd, userTable, &setIdx);
    // printf("------------------index : %d\n",idx);
    if (idx == -1 || idx < 0 || idx >= TIMEOUT_ARRAY)
    {
        syslog(LOG_ERR, "Invalid user_fd %d or index %d\n", user_fd, idx);
        fprintf(stderr, "Invalid user_fd %d or index %d\n", user_fd, idx);
        return -1;
    }

    // for (int i = 0; i < TIMEOUT_MAX; i++)
    // {
    //     if (netfdArray[idx][i] == user_fd)
    //     {
    //         timeout_array_add(netfdArray,userTable,timeoutArrayIndex,user_fd);
    //         user_table_change(user_fd,timeoutArrayIndex,userTable);
    //         netfdArray[idx][i] = -1;
    //         syslog(LOG_INFO,"Thread %ld: Moved user_fd %d from netfdArray[%d][%d] to netfdArray[%d][new_index]\n", pthread_self(), user_fd, idx, i, timeoutArrayIndex -1);
    //         printf("Thread %ld: Moved user_fd %d from netfdArray[%d][%d] to netfdArray[%d][new_index]\n", pthread_self(), user_fd, idx, i, timeoutArrayIndex -1);
    //         break;
    //     }
    // }

    if (netfdArray[idx][setIdx] != -1)
    {
        netfdArray[idx][setIdx] = -1;
        timeout_array_add(netfdArray, userTable, timeoutArrayIndex, user_fd,&setIdx);
        //user_table_change(user_fd, timeoutArrayIndex, userTable,&setIdx);
        syslog(LOG_INFO, "Thread %ld: Moved user_fd %d from netfdArray[%d][%d] to netfdArray[%d][new_index]\n", pthread_self(), user_fd, idx, setIdx, timeoutArrayIndex - 1);
        printf("Thread %ld: Moved user_fd %d from netfdArray[%d][%d] to netfdArray[%d][new_index]\n", pthread_self(), user_fd, idx, setIdx, timeoutArrayIndex - 1);
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
        syslog(LOG_INFO, "Thread %ld: Processing timeoutArrayIndex %d\n", pthread_self(), timeoutArrayIndex);
        printf("Thread %ld: Processing timeoutArrayIndex %d\n", pthread_self(), timeoutArrayIndex);
        timeoutArrayIndex = (timeoutArrayIndex + 1) % TIMEOUT_ARRAY;
        int current_index = timeoutArrayIndex;
        pthread_mutex_unlock(&timeout_index_mutex);
        timeout_array_delete(netfdArray, userTable, current_index);
    }
    return NULL;
}
