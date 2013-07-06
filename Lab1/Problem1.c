#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
  //Define an int* pointer variable, and create an array of 10 integers using malloc().  Then, assign values to that array, print their values, and free() the integers.
  int arraySize = 10;
  
  //Allocate space
  int* intArray  = malloc(arraySize*sizeof(int));
  int i;
  for(i = 0; i < arraySize; i++)
  {
    intArray[i] = i;
  }
  
  //Assign values
  for(i = 0; i < arraySize; i++)
  {
    intArray[i] = i;
  }
  
  //Print values
  for(i = 0; i < arraySize; i++)
  {
    printf("intArray[%d]=%d\n",i,intArray[i]);
  }
  
  free(intArray);
  
  return 0;
}
