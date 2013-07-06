#include <stdio.h>
#include <stdlib.h>

//Finally, write a program that, using malloc and realloc, creates an array of initial size n.  
//Write add(), remove() and get() functions for your array.  When adding beyond the end of the array, 
//reallocate space such that the array contains one more element.  Time your program for adding 
//100000 elements (or more).  Then modify the program such that it increases in size by a factor of 
//2 times the previous size.  Time it again.  What do you observe?


void add(int x);
void delete(int index); //remove declared in stdio.h
int get(int index);

int * array;
int arraySize;
int numElem;
int growFactor = 2;
int growConst = 0;
int intsToAdd = 100000000;

int main(int argc, char** argv)
{
  //Initialize
  arraySize = 10;
  numElem = 0;
  array = malloc(arraySize*sizeof(int));
  
  int i=0;
  for(i=0; i<intsToAdd; i++)
  {
    add(rand()%100);
  }
  
  return 0;
}

void add(int x)
{
  //printf("Add Called\n");
  if(numElem >= arraySize)
  {
    //Grow array
    //printf("Realloc Called!\n");
    int newSize = growFactor*arraySize + growConst;
    array = realloc(array,newSize*sizeof(int));
    arraySize = newSize;
  }
  
  //Just add to end
  array[numElem] = x;
  numElem++;

  return;
}

void delete(int index)
{
  //Copy all remaining elements down to take the place of the removed element
  int i;
  for(i=index; i < numElem-1; i++)
  {
    //Take element from i+1 and copy into i
    array[i] = array[i+1];
  }
  numElem--; //This ignores the remaining data in the now smaller array
  return;
}

int get(int index)
{
  return array[index];
}
