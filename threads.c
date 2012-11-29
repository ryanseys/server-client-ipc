#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define NUM_THREADS 10

void * thread_fun(void * arg) {
  int * val = arg;
  printf("fun stuff!%d\n", *val);
  sleep(1); //do work (notice no delay)

  /* Return value must not be on the stack, as the stack will
  be invalid and may be overwritten after our thread exits. */
  int * myretp = malloc(sizeof(int)); if (myretp == NULL) {
    perror("malloc error"); pthread_exit(NULL);
  }
  *myretp = (*arg) + 21;
  return myretp; /* Same as: pthread_exit(myretp); */
}

int main(int argc, char ** argv) {
  pthread_t threads[NUM_THREADS];
  int targs[NUM_THREADS];
  int i, ret;
  /* Create and start all threads. */
  for (i = 0; i < NUM_THREADS; i++) {
    targs[i] = i;
    ret = pthread_create(&(threads[i]), NULL, thread_fun, &(targs[i]) );
    if (ret == -1) {
      perror("pthread_create");
      exit(EXIT_FAILURE);
    }
  }
}

