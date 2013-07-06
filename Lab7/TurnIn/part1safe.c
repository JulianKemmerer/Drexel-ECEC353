#include <stdio.h>
#include <stdlib.h>
#include "csapp.h"

//Compile and run with gcc part1safe.c csapp.c -pthread -o part1safe && ./part1safe

struct ListNode
{
  int data;
  struct ListNode* next;
  struct ListNode* prev;
};

void append(struct ListNode* list, int data);
void printList(struct ListNode* list);
void printElement(struct ListNode* node);
void delete(struct ListNode* node);

//Global variable for list
struct ListNode * head;

#define NUM_THREADS 10

//Global lock for writing and deletion
pthread_mutex_t mutex_shared;

void * doAppend(void *threadid)
{
  int id;
  id = (int)threadid;
  
  //Append the thread id to the list
  append(head,id);
  
  //Exit properly
  pthread_exit(NULL);
}

int main(int argc, char** argv)
{
  //Linked list header
  head = Malloc(sizeof(struct ListNode));
  head->data = -1; //Negative one for header data (start appending at zero in threads)
  head->prev = NULL;
  head->next = NULL;
  
  //Init the lock
  pthread_mutex_init(&mutex_shared, NULL);
    
  //Spawn some threads to insert into the linked list
  pthread_t threads[NUM_THREADS];
  int t, rc;
  
  for(t=0; t<NUM_THREADS; t++)
  {
      rc = pthread_create(&threads[t], NULL, doAppend, (void *)t);
      if (rc)
      {
         printf("ERROR: return code from pthread_create() is %d\n", rc);
         exit(-1);
      }
   }
   
   //Wait for threads to finish
   void * status;
   for(t=0; t<NUM_THREADS; t++) 
   {
      rc = pthread_join(threads[t], &status);
      if (rc) 
      {
         printf("ERROR: return code from pthread_join() is %d\n", rc);
         exit(-1);
      }
      //printf("Main: completed join with thread %d having a status of %d\n",t,status);
   }
   
   //Print the list after all threads are done
   printList(head);
   
  //Exit main properly
  pthread_exit(NULL);
}

void append(struct ListNode* list, int data)
{
  //Lock
  pthread_mutex_lock(&mutex_shared);
  
  //Loop through list until end is found
  struct ListNode* iter = list;
  while(iter->next != NULL)
  {
    //Move to the next
    iter = iter->next;
  }
  //Current iter is last element
  //Allocate a new node
  struct ListNode* newNode = Malloc(sizeof(struct ListNode));
  //Initialize new node
  newNode->data = data;
  newNode->prev = iter;
  newNode->next = NULL;
  
  //Link old end of list to new node
  iter->next = newNode;
  
  //Unlock
  pthread_mutex_unlock(&mutex_shared);
}

void printList(struct ListNode* list)
{
  //Loop through list
  struct ListNode* iter = list;
  while(iter != NULL)
  {
    //Print data
    printf("%d ",iter->data);
    //Move to the next
    iter = iter->next;
  }
  //End with an end line
  printf("\n");
}

void printElement(struct ListNode* node)
{
  if(node != NULL)
  {
    printf("%d",node->data);
  }
}
void delete(struct ListNode* node)
{
  //Lock
  pthread_mutex_lock(&mutex_shared);
  
  //Delete this node by
  //   <- a ->   <- node ->  <- b ->
  struct ListNode* a = node->prev;
  struct ListNode* b = node->next;
  
  //Reroute the a node
  if(a != NULL)
  {
    a->next = b;
  }
  //Reroute the b node
  if(b != NULL)
  {
    b->prev = a;
  }
  
  //Delete the middle node
  Free(node);
  
  //Unlock
  pthread_mutex_unlock(&mutex_shared);
}

