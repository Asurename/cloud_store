#ifndef __PTHREADPOOL_H
#define __PTHREADPOOL_H
#include "tast_queue.h"
#include <mysql/mysql.h>
//�̳߳ؽṹ��
typedef struct threadpool_s {
    pthread_t* pthreads;
    int pthread_num;
    tast_queue* q;
}threadpool;
//�����̳߳ز����ṹ��
typedef struct queue_and_mysql {
    tast_queue* q;
    MYSQL* p_mysql;
}queue_and_mysql_t;
//�̳߳س�ʼ����������
threadpool* threadpool_init(int num);
//�̳߳�����
int threadpool_start(threadpool* thp, void* start_function(void* arg), void* arg);
//�̳߳�����
int threadpool_destroy(threadpool* thp);
//�̳߳�ֹͣ
int threadpool_stop(threadpool* thp);

#endif
