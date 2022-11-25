// Compile:  gcc THIS_FILE -pthread

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>  /* for sleep */

void * do_work(void *arg) {
  printf("I'm here\n");
}

int main() {
  pthread_t mythread;

  pthread_create( &mythread, NULL, do_work, NULL);
  // pthread_join() is like waitpid(); mythread is child thread (like pid arg of pthread_join)
  pthread_join( mythread, NULL );
  return 0;
}
