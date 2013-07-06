//Compile and run with gcc threadbarrier.o threadbarrierlibtest.c -pthread -o threadbarrierlibtest && ./threadbarrierlibtest

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define THREADS 10

void * threadedfunction(void *arg)
{
  int id = (int)arg;
  printf("Thread %d waiting at barrier\n",id);
  waitbarrier();
  printf("Thread %d got through.\n",id);
}

int main(int argc, char** argv)
{
  //Array of threads
  pthread_t threads[THREADS];
  
  initbarrier(THREADS);
  
  //Start threads
  int i;
  for(i = 0; i < THREADS; ++i)
  {
      //Wait between thread spawns to be
      //able to see each thread arrive at the barrier
      //Easier to 'read' the output this way
      sleep(2);
      if(pthread_create(&threads[i], NULL, &threadedfunction, (void*)i))
      {
	  printf("Could not create thread %d\n", i);
	  return -1;
      }
  }
  
  //Wait for threads
  for(i = 0; i < THREADS; ++i)
  {
      if(pthread_join(threads[i], NULL))
      {
	  printf("Could not join thread %d\n", i);
	  return -1;
      }
  }
  
  //Exit properly
  pthread_exit(NULL);
}
