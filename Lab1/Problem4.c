#include <stdio.h>
#include <stdlib.h>

//Modify this program to take in a linked list of structs that you create (with a int data element, and a struct ListNode* next pointer), and sort the linked list instead.

struct ListNode
{
  int data;
  struct ListNode* next;
  struct ListNode* prev;
};

void sort(struct ListNode * listHead, int size);
void bubble_sort(struct ListNode * listHead, int size, int lastelement);

int main(int argc, char** argv)
{
  int listSize = 10;
  //Create list head
  struct ListNode * listHead = malloc(sizeof(struct ListNode));
  listHead->prev = NULL;
  
  //Looping variables
  struct ListNode * current;
  int count = 1; //Head already created
  current = listHead;
  do
  {
    current->data = rand()%100; //Random data
    
    //If more nodes needed
    if(count < listSize)
    {
      //Allocate a new node and link next
      current->next = malloc(sizeof(struct ListNode));
      //Connect next nodes previous ptr to self
      current->next->prev = current;
      count++;
    }
    else
    {
      //This is the end
      current->next = NULL;
    }
    //Only assign the next value to current if next is not null
  }while((current->next != NULL) && (current = current->next));
 
  current = listHead;
  printf("Unsorted Linked List:\n");
  while(current != NULL)
  {
    printf("%d\n",current->data);
    current = current->next;
  }
  
  sort(listHead,listSize);
  
  current = listHead;
  printf("\nSorted Linked List:\n");
  while(current != NULL)
  {
    printf("%d\n",current->data);
    current = current->next;
  }
  
  return 0;
}

void sort(struct ListNode * listHead, int size)
{
  bubble_sort(listHead,size,size-1);
  return;
}

void bubble_sort(struct ListNode * listHead, int size, int lastelement)
{
  if(lastelement <0)
  {
    //Last element is first element - all elements sorted
    return; 
  }
  
  struct ListNode * current = listHead;
  struct ListNode * next = listHead->next;
  int index0 = 0;
  int index1 = 1;
  
  //Loop through each
  while((index1 <= lastelement) && (index1 < size) )
  {
    //If higher index is less, swap
    if( (next->data) < (current->data))
    {
      int old0 = current->data;
      int old1 = next->data;
      next->data = old0;
      current->data = old1;
    }
    index0++;
    index1++;
    current = next;
    next = current->next;
  }
  //The last element is now sorted, ignore it next time
  lastelement--;
  bubble_sort(listHead,size,lastelement);
}




