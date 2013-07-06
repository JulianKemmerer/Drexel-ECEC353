//Compile library with gcc -c threadbarrier.c -pthread -o threadbarrier.o

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


// Barrier variable
pthread_barrier_t barr;

void initbarrier(int threads)
{
  // Barrier initialization
  if(pthread_barrier_init(&barr, NULL, threads/2)) //Wait for half the threads
  {
      printf("Could not create a barrier\n");
      exit(-1);
  }
}

void waitbarrier()
{
    /* From man page:
       When  the required number of threads have called pthread_barrier_wait()
       specifying  the  barrier,  the  constant  PTHREAD_BARRIER_SERIAL_THREAD
       shall  be returned to one unspecified thread and zero shall be returned
       to each of the remaining threads. At this point, the barrier  shall  be
       reset  to  the state it had as a result of the most recent pthread_bar-
       rier_init() function that referenced it.
    */
  
    // Synchronization point
    int rc = pthread_barrier_wait(&barr);
    if(rc != 0  && rc != PTHREAD_BARRIER_SERIAL_THREAD)
    {
        printf("Could not wait on barrier\n");
        exit(-1);
    }
}
