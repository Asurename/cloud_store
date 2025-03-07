#include "pthreadpool.h"
#include "../config/config.h"

//��ʼ���̳߳� 
threadpool* threadpool_init(int num) {
    threadpool* thp = (threadpool*)malloc(sizeof(threadpool));
    thp->pthreads = (pthread_t*)malloc(sizeof(pthread_t) * num);
    thp->pthread_num = num;
    memset(thp->jwt,0,sizeof(thp->jwt));
    thp->q = tast_queue_create();
    return thp;
}

//--------------------------------------------------------------------------------------------------//
//�̳߳�����
int threadpool_start(threadpool* thp, void* start_function(void* arg), void* arg) {
    int err;
    for (int i = 0; i < thp->pthread_num; i++) {
        err = pthread_create(&thp->pthreads[i], NULL, start_function, arg);
        if (err == -1) {
            error(0, errno, "pthread_create");
            return -1;
        }
    }
    printf(ANSI_COLOR_CYAN);
    printf("[PTHREAD_POOL]%d thread be create and start work\n",thp->pthread_num);
    printf(ANSI_COLOR_RESET);
    return 0;
}

//--------------------------------------------------------------------------------------------------//
//�̳߳�����
int threadpool_destroy(threadpool* thp) {
    free(thp->pthreads);
    tast_queue_destroy(thp->q);
    free(thp);
    return 0;
}
