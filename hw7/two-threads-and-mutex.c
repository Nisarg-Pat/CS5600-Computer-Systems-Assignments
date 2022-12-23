#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t mutex1;
pthread_mutex_t mutex2;
pthread_t thread1, thread2;

void * thread_worker1(void *forks_arg) {
  int i;
  for (i = 0; i < 100; i++) {
    pthread_mutex_lock(&mutex2);
    pthread_mutex_lock(&mutex1);
    pthread_mutex_unlock(&mutex1);
    pthread_mutex_unlock(&mutex2);
    sleep( rand() % 3 ); // Simulate useful work
    printf("WORKER 1 DID A TASK.\n");
  }
  return NULL;
}

void * thread_worker2(void *forks_arg) {
  int i;
  for (i = 0; i < 100; i++) {
    pthread_mutex_lock(&mutex1);
    pthread_mutex_lock(&mutex2);
    pthread_mutex_unlock(&mutex2);
    pthread_mutex_unlock(&mutex1);
    sleep( rand() % 3 ); // Simulate useful work
    printf("WORKER 2 DID A TASK.\n");
  }
  return NULL;
}

int main(int argc, char* argv[]) {
    pthread_mutex_init(&mutex1, NULL);
    pthread_mutex_init(&mutex2, NULL);

    pthread_create(&thread1, NULL, &thread_worker1, NULL);
    pthread_create(&thread2, NULL, &thread_worker2, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    return 0;
}

