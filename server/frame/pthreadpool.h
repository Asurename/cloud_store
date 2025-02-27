#ifndef __PTHREADPOOL_H
#define __PTHREADPOOL_H
#include "tast_queue.h"

//线程池结构体
typedef struct threadpool_s {
    pthread_t* pthreads;
    int pthread_num;
    tast_queue* q;
}threadpool;
//线程池初始化，不启动
threadpool* threadpool_init(int num);
//线程池启动
int threadpool_start(threadpool* thp, void* start_function(void* arg), void* arg);
//线程池销毁
int threadpool_destroy(threadpool* thp);
//线程池停止
int threadpool_stop(threadpool* thp);

#endif
