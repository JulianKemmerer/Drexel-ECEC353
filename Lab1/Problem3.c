#include <stdio.h>
#include <stdlib.h>

void sort(int* a, int size);
void bubble_sort(int* a, int size, int lastelement);

int main(int argc, char** argv)
{
  //Populate array
  int arraySize = 20;
  int * array = malloc(arraySize * sizeof(int));
  int i = 0;
  for(i = 0; i < arraySize; i++)
  {
    array[i] = rand() %100;
  }
  
  for(i = 0; i < arraySize; i++)
  {
    printf("OriginalArray[%d]=%d\n",i,array[i]);
  }
  
  sort(array,arraySize);
  
  for(i = 0; i < arraySize; i++)
  {
    printf("SortedArray[%d]=%d\n",i,array[i]);
  }
  
  return 0;
}

//Write a function sort() that takes in an int* a and int size, and sorts the array using pointer arithmetic.
void sort(int* a, int size)
{
  bubble_sort(a,size,size-1);
  return;
}

void bubble_sort(int* a, int size, int lastelement)
{
  if(lastelement <0)
  {
    //Last element is first element - all elements sorted
    return; 
  }
  
  int index0 = 0;
  int index1 = 1;
  //Loop through each
  while((index1 <= lastelement) && (index1 < size))
  {
    //If higher index is less, swap
    if(a[index1] < a[index0])
    {
      int old0 = a[index0];
      int old1 = a[index1];
      a[index1] = old0;
      a[index0] = old1;
    }
    index0++;
    index1++;
  }
  //The last element is now sorted, ignore it next time
  lastelement--;
  bubble_sort(a,size,lastelement);
}




