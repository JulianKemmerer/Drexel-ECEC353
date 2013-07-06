#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

//Function prototypes
//Construct the hash table
void constructHashTable(FILE * file);

//Return the hash value for a word
//Hash changes word to all lowercase and removes newlines
int hash(char * word);

//Return the ascii sum for a word
int ascii_sum(char * word);

//Dumb function that finds the max hash_base value for use in constructing
//and appropritely scaling hash values, useful when considering
//how many 'buckets' you want.
//Dumb as it reads the file in advance - slow? useful? we'll see.
int hash_base_max(char * path);

//The actual code to calculate the'base' / unscaled hash value 
int hash_base(char * word);

//Store the max value return from hash_base_max()
int hashBaseMaxValue = 1;

//Number of 'buckets' / linked lists to store
//Define as to use in static array declaration
#define numHashBuckets 1000 //Seg faults higher than 1030

//Functions to print anagrams for a word
void printAnagrams(char * word);
bool isAnagram(char * input, char * toCheck);

struct LinkNode
{
  char * word;
  struct LinkNode * next;  
};

//Protype for dealing with hashtable and linked lists
//Double star to modify heads ptr value
void append(struct LinkNode ** head,char * word);

struct HashTable
{
  //The scaling done as part of the hash function actually produces numHashBuckets +1
  //possible values so allocate (statically) an array of linked lists that is
  //numHashBuckets +1 in size
  struct LinkNode nodeHeads[numHashBuckets +1];
};
struct HashTable table;
void printHashTable(struct HashTable * table);

//Arg 0 is program name, arg 1 is dictionary file path, arg 2 is word to find anagrams for
int main(int argc, char** argv) 
{
  //Check for input file
  if(argv[1] == NULL)
  {
     printf("Please provide an input dictionary file.\n");
     return -1;
  }
  
  //Assign the max hash_base value for use later
  //See prototype for why this is probably dumb and slow.
  hashBaseMaxValue = hash_base_max(argv[1]);
  
  //Try to open the file read only
  FILE* dict = fopen(argv[1], "r"); //open the dictionary for read-only access
  //Check that it opened correctly
  if(dict == NULL) 
  {
      printf("Failed to open: %s\n",argv[1]);
      return -1;
  }
  
  //Dict file is provided and opened sucessfully, construct the table
  constructHashTable(dict);
  
  //printHashTable(&table);
  
  //Check that a word is provided
  if(argv[2] == NULL)
  {
    printf("Why do you bother running this program without providing a word to find anagrams for...?\n");
    return -1;
  }
  else
  {
    //Word was provided, print anagrams
    printAnagrams(argv[2]);
  }
  
  fclose(dict);
}

void printAnagrams(char * word)
{
  //Hash the word
  int hashvalue = hash(word);
  
  //Find the correct spot within the table and loop through the appropriate linked list
  struct LinkNode * iterNode = &(table.nodeHeads[hashvalue]); //Start iterator at the head node's address
  while(iterNode !=NULL)
  {
    if( isAnagram(word,iterNode->word) )
    {
      //Print word
      printf("%s\n",iterNode->word);
    }
    //Go to next item in linked list
    iterNode = iterNode->next;
  }
}

bool isAnagram(char * word0, char * word)
{
  //Make a copy of word for modifing, word1
  size_t stringSize = (strlen(word)+1) * sizeof(char);
  char * word1 = malloc( stringSize );
  if(word1 !=NULL)
  {
    strncpy(word1,word,stringSize);
  }
  
  //Quick checks for length
  //Store lengths for loops
  //Lengths should the same
  int word0len = strlen(word0);
  int word1len = strlen(word1);
  if(word0len != word1len)
  {
    return false;
  }
  //And ascii sum
  if(ascii_sum(word0) != ascii_sum(word1))
  {
    return false;
  }
  
  //Final check
  //Loop through word0
  int i,j;
  bool charFound;
  for(i=0; i< word0len; i++)
  {
    //Find char word0[i] in word1
    //Loop through word1 until word0[i] is found
    charFound = false;
    for(j=0; j < word1len; j++)
    {
      if(word0[i] ==word1[j])
      {
	word1[j] = ' '; //space to mark
	charFound = true;
	break;
      }
    }
    if(charFound == false)
    {
    //Made it through a loop without finding a matching char
    return false;
    }
  }
  return true;
}

void constructHashTable(FILE * file)
{
  // Read each line of the file
  char word[128];
  while(fgets(word, sizeof(word), file) != NULL) 
  {
      int hashvalue = hash(word);//Hash changes word to all lowercase and removes newlines
      //Build the hash table
      struct LinkNode * node = &(table.nodeHeads[hashvalue]);
      append(&node,word);
  }
  return;
}

void printHashTable(struct HashTable * table)
{
  //Debug function
  int i;
  for(i=0; i <= numHashBuckets +1;i++)
  {
    printf("\nHASH VALUE: %d= ",i);
    struct LinkNode * iterNode = &(table->nodeHeads[i]);
    while(iterNode != NULL)
    {
      printf("%s\n",iterNode->word);
      iterNode = iterNode->next;
    }
  }
}

//Double star to modify heads ptr value -----------------------------------FIX ME ** NOT NEEDED.
void append(struct LinkNode ** head,char * word)
{
  //If no words are in the list, just change head values
  if( (*head)->word ==NULL) //next should be never a valid ptr when word is not
  {
    //Allocate space for word in container
    size_t stringSize = (strlen(word)+1) * sizeof(char);
    (*head)->word = malloc( stringSize );
    if((*head)->word != NULL)
    {
      //Copy from word passed in into new allocated space
      strncpy((*head)->word,word,stringSize);
      
      //Next should already be null...but just to be sure
      (*head)->next = NULL;
    }
    else
    {
      printf("Malloc Error.");
      return;
    }
  }
  //Nodes exists, add new node to beginning of list
  else
  {
    //Need new LinkNode (container)
    struct LinkNode * newNode = malloc(sizeof(struct LinkNode));
    if(newNode != NULL)
    {
      //Allocate space for new word in new container
      size_t stringSize = (strlen(word)+1) * sizeof(char);
      newNode->word = malloc( stringSize );
      if(newNode->word != NULL)
      {
	//Copy from word passed in into new allocated space
	strncpy(newNode->word,word,stringSize);
	
	//Append new node between head and first node	
	newNode->next = (*head)->next;
	(*head)->next = newNode;
      }
      else
      {
	printf("Malloc Error.");
	return;
      }
    }
    else
    {
      printf("Malloc Error.");
      return;
    }
  }
}

int hash(char * word)
{
  //Change input to lower case
  int i;
  int len = strlen(word);
  //Don't modify last char - null char
  for(i=0; i < len; i++)
  {
    //To get rid of newline change it to null term
    if(word[i]=='\n')
    {
      word[i]='\0';
    }
    else
    {
      word[i] = tolower(word[i]);
    }
  }
  
  //printf("Hashing: %s\n",word);
  //We want to preserve the 0-1 fraction that comes out of the division
  //So cast as float to force
  //Then scale the fraction to the scaled number of buckets
  //A 0-1 fraction , when scaled, then cast to an int becomes a scaled int
  //Ex. 0-1 scaled by 100 will produce integers in the range 0 to 100
  //So really numHashBuckets +1 is the real number of buckets 
  //printf("Returning: %d\n",(int)((hash_base(word)/(float)hashBaseMaxValue)*numHashBuckets));
  return (hash_base(word)/(float)hashBaseMaxValue)*numHashBuckets; 
}

int ascii_sum(char * word)
{
  //Simple function to return ascii sum
  int i = 0;
  int sum = 0;
  //Loop to end of word
  while(word[i] !='\0' && word[i] !='\n' && word[i] != EOF)
  {
    sum+=word[i];
    i++;
  }
  return sum;
}

int hash_base_max(char * path)
{
  //This function is 'stupid' in that it reads the file
  //In it's entirety to get the max hash_base value
  //That is later used to calculate the scaled hash value
  FILE* file = fopen(path, "r");
  if(file == NULL) 
  {
      printf("Failed to open: %s\n",path);
      return 1; //Not zero for division later
  }
  
  // Read each line of the file
  char word[128];
  int max = 1;
  while(fgets(word, sizeof(word), file) != NULL) 
  {
      int value = hash_base(word);
      if(value > max)
      {
	max = value;
      }
  }
  fclose(file);
  return max;
}

//Base hashing function
int hash_base(char * word)
{
  return ascii_sum(word) * strlen(word);
}

