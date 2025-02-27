#ifndef __TAST_QUEUE_FUNC_H
#define __TAST_QUEUE_FUNC_H
#include <func.h>
//任务队列节点，啥都能挂(void*)
typedef struct node {
    void* tast_t;
    struct node* next_t;
}node_t;

//任务队列
typedef struct {
    node_t* pfornt;
    node_t* prear;
    int size;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int flag;
}tast_queue;
//初始化
tast_queue* tast_queue_create(void);
//添加任务
int tast_queue_push(tast_queue* q, void* t);
//取出任务
void* tast_queue_pop(tast_queue* q);
//计算任务数量
int tast_queue_size(tast_queue* q);
//销毁队列
int tast_queue_destroy(tast_queue* q);

#endif
