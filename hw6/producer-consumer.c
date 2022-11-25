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

int main() {
 // ... uses pthread_create to start producer and consumer
 // You must add that.
 // WARNING:  the primary thread runs main().  When main exits, the primary
 //             thread exits, unless you call 'pthread_join()' to
 //             have 'main' wait on the other threads before exiting.
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
