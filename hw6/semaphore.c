#include <pthread.h>
#include <unistd.h>
#include "header.h"

int sem_init(struct sem_t *sem, int ignore, int init) {
  sem->count = init;
  sem->waiting = 0;
  pthread_mutex_init(&(sem->mutex), NULL);
  pthread_cond_init(&(sem->condition), NULL);
}

int sem_wait(struct sem_t *sem) {
  pthread_mutex_lock(&(sem->mutex));
  sem->count -= 1;
  if(sem->count<0) {
    sem->waiting+=1;
    while(-(sem->count) == sem->waiting) {
      pthread_cond_wait(&(sem->condition), &(sem->mutex));
    }
    sem->waiting -= 1;
  }
  pthread_mutex_unlock(&(sem->mutex));
}

int sem_post(struct sem_t *sem) {
  pthread_mutex_lock(&(sem->mutex));
  sem->count += 1;
  if(sem->waiting > 0) {
    pthread_cond_signal(&(sem->condition));
  }
  pthread_mutex_unlock(&(sem->mutex));
}
