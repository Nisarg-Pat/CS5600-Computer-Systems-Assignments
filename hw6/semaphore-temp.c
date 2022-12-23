#include <pthread.h>
#include <unistd.h>
#include "header.h"

void block(struct sem_t *sem);

int sem_init(struct sem_t *sem, int ignore, int init) {
  sem->count = init;
  pthread_mutex_init(&(sem->mutex), NULL);
}

int sem_post(struct sem_t *sem) {
  pthread_mutex_lock(&(sem->mutex));
  sem->count += 1;
  pthread_mutex_unlock(&(sem->mutex));
}

int sem_wait(struct sem_t *sem) {
  pthread_mutex_lock(&(sem->mutex));
  sem->count -= 1;
  if (sem->count < 0) {
    pthread_mutex_unlock(&(sem->mutex));
    block(sem);
    pthread_mutex_lock(&(sem->mutex));
  }
  pthread_mutex_unlock(&(sem->mutex));
}

void block(struct sem_t *sem) {
  int mycount;
  while(1) {
    pthread_mutex_lock(&(sem->mutex));
    mycount = sem->count;
    if (mycount >= 0) {
      pthread_mutex_unlock(&(sem->mutex));
      return;
    }
    pthread_mutex_unlock(&(sem->mutex));
    sleep(1);
  }
}
