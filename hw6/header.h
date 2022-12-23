#include<pthread.h>
struct sem_t {
  int count;
  int waiting;
  pthread_mutex_t mutex;
  pthread_cond_t condition;
};

int sem_init(struct sem_t *sem, int ignore, int init);
int sem_post(struct sem_t *sem);
int sem_wait(struct sem_t *sem);
