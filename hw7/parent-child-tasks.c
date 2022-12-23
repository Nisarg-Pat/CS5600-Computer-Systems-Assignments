#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#define INITIALIZE (-1)
int idx = -1;
int length = -1;
int *buffer = NULL;
int next_prime(int x);

sem_t sem_prod; // producer
sem_t sem_cons; // consumer

void * child_worker(void *dummy) {
  int i;

  idx = INITIALIZE; // Ask parent to create buffer
  length = 100; // Ask parent to make buffer of size 100
  for (i = 0; i < length; i++) {
    sem_wait(&sem_prod);
    sleep( rand() % 3 ); // Simulate time to compute prime
    if (idx >= length) { return; }
    if (idx == INITIALIZE) {
      idx = 0;
      buffer[idx++] = 2;
    } else {
      buffer[idx] = next_prime(buffer[idx-1]);
      idx++;
    }
    sem_post(&sem_cons);
  }
  return NULL;
}

int main() {
  pthread_t child;
  sem_init(&sem_prod, 0, 0);
  sem_init(&sem_cons, 0, 0);
  pthread_create( &child, NULL, child_worker, NULL); 
  sleep( rand() % 3 ); // Simulate useful work

  while (1) {
    // Child sets idx and length as global variables.
    if (idx == INITIALIZE) {
      buffer = malloc(length*sizeof(int));
      // Maybe don't need an assert here.  Let it segfault.
      idx = 0;
    } else {
      printf("%d, ", buffer[idx-1]);
      fflush(stdout);
    }
    sem_post(&sem_prod);
    // Simulate useful work
    sleep( rand() % 3 ); // Simulate useful work before waiting
    sem_wait(&sem_cons);
    if (idx > length) {
      break;
    }
  }

  pthread_join(child, NULL); 
  return 0;
}

// ==========================================================
// FIND NEXT PRIME:

int is_prime(int x) {
  int i;
  for (i = 2; i < x; i++) {
    if (x % i == 0) {
      return 0; // not prime
    }
  }
  return 1; // is prime
}

int next_prime(int x) {
  int i;
  for (i = x+1; i < x+1000000; i++) {
    if (is_prime(i)) {
      return i;
    }
  }
  fprintf(stderr, "Internal error!\n");
  exit(1);
}
