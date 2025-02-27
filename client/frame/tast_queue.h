#ifndef __TAST_QUEUE_FUNC_H
#define __TAST_QUEUE_FUNC_H
#include <func.h>
//������нڵ㣬ɶ���ܹ�(void*)
typedef struct node {
    void* tast_t;
    struct node* next_t;
}node_t;

//�������
typedef struct {
    node_t* pfornt;
    node_t* prear;
    int size;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int flag;
}tast_queue;
//��ʼ��
tast_queue* tast_queue_create(void);
//�������
int tast_queue_push(tast_queue* q, void* t);
//ȡ������
void* tast_queue_pop(tast_queue* q);
//������������
int tast_queue_size(tast_queue* q);
//���ٶ���
int tast_queue_destroy(tast_queue* q);

#endif
