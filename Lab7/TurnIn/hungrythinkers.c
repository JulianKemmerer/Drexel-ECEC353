//Compile and run with gcc hungrythinkers.c -pthread -o hungrythinkers && ./hungrythinkers

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define HUNGRYGUYS 5

//Locks
pthread_mutex_t chopsticks[HUNGRYGUYS]; //One per guy

void printhelpertabs(int n)
{
  int i;
  for(i=0; i<n ;i++)
  {
    printf("\t");
  }
}


void * eat(void *arg)
{
  int id = (int)arg;
  int eatcount = 0;
  
  //If you are the first philospher, look in the opposite direction for your fork
  //Normally philosopher i looks for chop stick i
  //First philosopher looks for i-1 but sinc i=0 they actually reach for the 'last' chop stick
  int chopsticknum1,chopsticknum2;
  
  //Keep eating a set number of times
  while(eatcount < 2)
  {
    if(id ==0)
    {
      //last chopstick index
      chopsticknum1 = HUNGRYGUYS-1;
    }
    else
    {
      //All other philosphers reach for 'their' index chop stick
      chopsticknum1 = id;
    }
    
    //Try to pickup the chop stick (lock)
    printhelpertabs(id);printf("Philosopher %d is reaching for first chop stick %d.\n",id,chopsticknum1);
    pthread_mutex_lock(&chopsticks[chopsticknum1]);
    printhelpertabs(id);printf("Philosopher %d got first chop stick %d.\n",id,chopsticknum1);
    
    //If you've gotten to this point you got one chop stick
    //Try to pick up the other
    //For philosopher 0 this is index 0
    //Everyone else it is index id-1
    if(id ==0)
    {
      //last chopstick index
      chopsticknum2 = 0;
    }
    else
    {
      //All other philosphers reach for 'their' index chop stick
      chopsticknum2 = id -1;
    }
    
    //Try to pickup second one
    printhelpertabs(id);printf("Philosopher %d is reaching for second chop stick %d.\n",id,chopsticknum2);
    pthread_mutex_lock(&chopsticks[chopsticknum2]);
    printhelpertabs(id);printf("Philosopher %d got second chop stick %d.\n",id,chopsticknum2);
    
    //If you got here, you are eating
    printhelpertabs(id);printf("Philosopher %d is getting the nom on.\n",id);
    sleep(1);
    
    //Put down both forks after eating
    printhelpertabs(id);printf("Philosopher %d put down first chop stick %d.\n",id,chopsticknum1);
    pthread_mutex_unlock(&chopsticks[chopsticknum1]);
    printhelpertabs(id);printf("Philosopher %d put down second chop stick %d.\n",id,chopsticknum2);
    pthread_mutex_unlock(&chopsticks[chopsticknum2]);
    //Increment eat counter
    eatcount++;
  }
}

int main(int argc, char** argv)
{
  printf("Tab levels are there to help pick out actions of certain philosophers.\n");
  printf("(Not perfect because there are no locks on printing but slightly helpful)\n");
  
  //Array of threads
  pthread_t threads[HUNGRYGUYS];
  
  int i;
  //Init locks
  for(i = 0; i < HUNGRYGUYS; ++i)
  {
    pthread_mutex_init(&chopsticks[i], NULL);
  }
   
  //Start threads
  for(i = 0; i < HUNGRYGUYS; ++i)
  {
      if(pthread_create(&threads[i], NULL, &eat, (void*)i))
      {
	  printf("Could not create thread %d\n", i);
	  return -1;
      }
  }
  
  //Wait for threads
  for(i = 0; i < HUNGRYGUYS; ++i)
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
