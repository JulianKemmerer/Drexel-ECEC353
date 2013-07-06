#include <stdio.h>
#include <stdlib.h>
#include "csapp.h"

//Compile and run with gcc part1unsafe.c csapp.c -pthread -o part1unsafe && ./part1unsafe

struct ListNode
{
  int data;
  struct ListNode* next;
  struct ListNode* prev;
};

void append(struct ListNode* list, int data);
void printList(struct ListNode* list);

//Global variable for list
struct ListNode * head;

#define NUM_THREADS 10

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

