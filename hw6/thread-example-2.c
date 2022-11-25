// Compile:  gcc THIS_FILE -pthread

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>  /* for sleep */

void * do_work(void *arg) {
  printf("I'm here\n");
  printf( "arg: %d\n", *(int *)arg);  // C cast:  convert type of arg to "int *"
  // return retval;  /* not used */
}

int main() {
  pthread_t mythread;
  void *retval;  // could be used by pthread_join, but we're using NULL
  int value = 17;
  int * myarg = &value;

  pthread_create( &mythread, NULL, do_work, myarg);
  // pthread_join() is like waitpid(); mythread is child thread (like pid arg of pthread_join)
  pthread_join( mythread, NULL );
  return 0;
}
