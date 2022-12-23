// This is a partial implementation of producer-consumer.  Fill in the rest.

// For debugging in GDB with threads: methodology 1
//   (gdb) info threads
//   (gdb) thread 2
//   (gdb) # and so on for other threads
//   (gdb) where # each thread has its own stack
//   (gdb) frame 2  # to go to call frame 2

// For debugging in GDB: methodology 2
//   (gdb) break consumer
//   (gdb) run
//   (gdb) print buf
//   (gdb) next
//   (gdb) print buf  # and so on

#include <stdio.h>
#include <pthread.h> // Needed for pthread_mutex_lock(), etc.

#include "header.h"
#include <stdlib.h> //for rand
#include <unistd.h> //for sleep

typedef struct sem_t sem_t;
// You have to:
// 1.  declare sem_t for your implementation
// 2.  Add to 'main':  sem_init, pthread_create, pthread_join
// 3.  define push_buf() and take_from_buf()
// 4.  link this with your semaphore.o (for your semaphore.c file),
//     which should defines sem_init, sem_wait, sem_post.
//     Note that 'partial_solution.c' is available for you to implement
//     sem_init, sem_wait, sem_post well enough for 1 producer, 1 consumer.

sem_t sem_producer;  // Should count number of empty slots available
sem_t sem_consumer;  // Should count number of items in the buffer
pthread_mutex_t mut_buf = PTHREAD_MUTEX_INITIALIZER;  // Lock for anybody touching buf


void* producer(void* arg);
void* consumer(void* arg);
void push_buf(int val);
int take_from_buf();

#define BUFFER_SIZE 5
int buffer[BUFFER_SIZE];
int producer_index;
int consumer_index;

int main() {
 // ... uses pthread_create to start producer and consumer
 // You must add that.
 // WARNING:  the primary thread runs main().  When main exits, the primary
 //             thread exits, unless you call 'pthread_join()' to
 //             have 'main' wait on the other threads before exiting.
  pthread_t producer_thread, consumer_thread;
  producer_index = 0;
  consumer_index = 0;
  sem_init(&sem_producer, 0, BUFFER_SIZE);
  sem_init(&sem_consumer, 0, 0);
  pthread_create(&producer_thread, NULL, producer, NULL);
  pthread_create(&consumer_thread, NULL, consumer, NULL);
  pthread_join(producer_thread, NULL);
  pthread_join(consumer_thread, NULL);
  while (1);  // Don't let the primary thread exit
}

void *producer(void *arg) {
  int work_item = 1;
  while (1) {
    sleep( rand() % 5 );
    sem_wait(&sem_producer);  // Wait for empty slots
    pthread_mutex_lock(&mut_buf);
      push_buf(work_item++);  // inside critical section with mut_buf lock
    pthread_mutex_unlock(&mut_buf);
    sem_post(&sem_consumer);  // Tell the consumer there's a new work item
  }
}

// Exactly the same, but the inverse:
void *consumer(void *arg) {
  while (1) {
    int work_item;
    sleep( rand() % 5 );
    sem_wait(&sem_consumer);
    pthread_mutex_lock(&mut_buf);
      work_item = take_from_buf();
    pthread_mutex_unlock(&mut_buf);
    sem_post(&sem_producer);

    printf("%d ", work_item);
    fflush(stdout);  // Force printing now; don't wait for the newline
  }
}

void push_buf(int val) {
  buffer[producer_index] = val;
  //printf("Produced %d at %d\n", val, producer_index);
  fflush(stdout);
  producer_index = (producer_index+1)%BUFFER_SIZE;
}

int take_from_buf() {
  int return_val = buffer[consumer_index];
  //printf("Consumed %d from %d\n", return_val, consumer_index);
  fflush(stdout);
  consumer_index = (consumer_index+1)%BUFFER_SIZE;
  return return_val;
}
