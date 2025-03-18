#include <stdlib.h>
#include <func.h>
#include "tast_queue.h"
//--------------------------------------------------------------------------------------------------//
//创建队列
tast_queue* tast_queue_create(void) {
    tast_queue* q = (tast_queue*)malloc(sizeof(tast_queue));
    node_t* head = (node_t*)malloc(sizeof(node_t));
    head->tast_t = NULL;
    head->next_t = NULL;

    q->pfornt = head;
    q->prear = head;
    q->size = 0;
    q->flag = 0;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond, NULL);
    return q;
}

//--------------------------------------------------------------------------------------------------//
//入队
int tast_queue_push(tast_queue* q, void* t) {
    pthread_mutex_lock(&q->mutex);
    node_t* node = (node_t*)malloc(sizeof(node_t));
    node->tast_t = t;
    node->next_t = NULL;
    q->prear->next_t = node;
    q->prear = q->prear->next_t;
    q->size++;
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
    return 0;
}


//--------------------------------------------------------------------------------------------------//
//出队
void* tast_queue_pop(tast_queue* q) {
    pthread_mutex_lock(&q->mutex);
    while (q->size == 0) {
        pthread_cond_wait(&q->cond, &q->mutex);
    }
    void* t = q->pfornt->next_t->tast_t;
    node_t* p = q->pfornt->next_t;
    q->pfornt->next_t = q->pfornt->next_t->next_t;
    free(p);
    q->size--;
    if (q->size == 0) {
        q->prear = q->pfornt;
    }
    pthread_mutex_unlock(&q->mutex);
    return t;
}

//--------------------------------------------------------------------------------------------------//
//队列长度
int tast_queue_size(tast_queue* q) {
    pthread_mutex_lock(&q->mutex);
    int size = q->size;
    pthread_mutex_unlock(&q->mutex);
    return size;
}

//--------------------------------------------------------------------------------------------------//
//摧毁队列
int tast_queue_destroy(tast_queue* q) {
    for (int i = 0; i < q->size; i++) {
        free(tast_queue_pop(q));
    }
    free(q);
    return 0;
}
