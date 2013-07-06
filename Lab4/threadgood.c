#include <pthread.h>
#include <stdio.h>
#define NUM_THREADS     1000
//Compile and run with $ gcc -pthread threadgood.c -o threadgood && ./threadgood

//Write a program that consists of one threaded function that you invoke via 1000 pthreads.  

int shared = 0;
pthread_mutex_t mutex_shared;

void * increment(void *arg)
{
  //That threaded function should increment a shared variable (initialized to 0), 100 times each.
  //100? 1000 times each... for 1000000 to be outcome
  int i;
  for(i=0; i<1000; i++)
  {
    pthread_mutex_lock(&mutex_shared);
    shared++;
    pthread_mutex_unlock(&mutex_shared);
  }
  pthread_exit(NULL);
}

int main (int argc, char *argv[])
{
  void *status;
   pthread_mutex_init(&mutex_shared, NULL);
   pthread_t threads[NUM_THREADS];
   int i;
   int rc;
   for(i=0; i<NUM_THREADS; i++)
   {
      rc = pthread_create(&threads[i], NULL, increment, (void *)NULL);
      if (rc){
         printf("ERROR; return code from pthread_create() is %d\n", rc);
         exit(-1);
      }
   }
   
   /* Wait on the other threads */
  for(i=0; i<NUM_THREADS; i++)
  {
    pthread_join(threads[i],&status);
  }
   
   printf("Shared=%d\n",shared);

   /* Last thing that main() should do */
   pthread_exit(NULL);
}