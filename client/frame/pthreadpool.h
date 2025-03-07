#ifndef __PTHREADPOOL_H
#define __PTHREADPOOL_H
#include "tast_queue.h"

//�̳߳ؽṹ��
typedef struct threadpool_s {
    char jwt[4096];
    pthread_t* pthreads;
    int pthread_num;
    tast_queue* q;
}threadpool;
//�̳߳س�ʼ����������
threadpool* threadpool_init(int num);
//�̳߳�����
int threadpool_start(threadpool* thp, void* start_function(void* arg), void* arg);
//�̳߳�����
int threadpool_destroy(threadpool* thp);
//�̳߳�ֹͣ
int threadpool_stop(threadpool* thp);

#endif
