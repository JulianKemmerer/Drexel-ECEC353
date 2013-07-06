#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
  //Using malloc, create a char** pointer that contains 10 char*'s, then in a loop, 
  //initialize each of the 10 char*'s in a loop to a char array of size 15, 
  //and initialize each to a word of your choice (don't forget the null terminator \0) -- and print them to screen.
  int numStrings = 10;
  int numChars = 15;
  
  //Allocate container for strings (Char*s)
  char** strings = malloc(numStrings * sizeof(char*));
  //Allocate a string of set size in each position
  int i = 0;
  for(i = 0; i < numStrings; i++)
  {
    strings[i] = malloc(numChars * sizeof(char));
  }
  
  //Initilize each string
  for(i = 0; i < numStrings; i++)
  {
    sprintf(strings[i],"WOW! %d\0",i);
  }
  
  //Print values
  for(i = 0; i < numStrings; i++)
  {
    //Print string
    printf("String %d: %s\n",i,strings[i]);
  }
  
  //Free each string
  for(i = 0; i < numStrings; i++)
  {
    free(strings[i]);
  }
  
  //Free the container
  free(strings);  
  
  return 0;
}
