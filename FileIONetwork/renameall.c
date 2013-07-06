#include <stdio.h>
#include <stdlib.h>
#include "csapp.h"
//Used for stat
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
//Used for directories
#include <dirent.h>

//Compile with gcc -pthread renameall.c ../csapp.c -o renameall

//Wrapper with errors for rename function
void Rename(char * old, char * new);
//Double star to modify the char *  (string)
//Returns extra chars added or removed
int strReplaceAll(char ** source, char * toFind, char * replacement);
//Directory wide rename
void dirWideRename(char * dirPath, char * toFind, char * replacement);
//File wide rename
void fileWideRename(char * filePath, char * toFind, char * replacement);
//Read file and replace - writes to tmp file
void fileReadReplace(char * filePath, char * toFind, char * replacement);
//Get a tmp file name
char * getTmpFileName(char * base);
//Remove files for dirs
void Remove(char * filename);
//Replace once
int strReplaceOnce(int * startIndex, char ** source, char * toFind, char * replacement);
//Move chars around in string
void shiftChars(char ** source , int windowIndex, int windowSize, int shamt);
//Check if all chars in a portion of string are the same
int sameChars(int sourceindex, char * source, char * toFind);
//Recursive function for directory navigation
void recursiveFileDirProcess(char * dir, char * file,char * toFind, char * replacement );

//Commandline args
//      0        1        2             3
//$ renameall strFind strReplacement strDirFile

int main(int argc, char** argv)
{
    if(!(argc >=4) ) //If commandline args are not provided
    {
      printf("Need to provide 'to find' string, 'replacement' string, and file or directory\n");
      return -1;
    }
    
    //Check type of input
    //Create stat struct to get info, and int to store status
    int filestatus;
    struct stat fileinfo;
    //Populate struc and status
    filestatus = stat(argv[3],&fileinfo);
    
    if(filestatus !=0)
    {
      fprintf(stderr, "Error in opening %s . Return code: %d\n",argv[3] ,filestatus);
      return;
    }
    
    //Check if file or directory
    if (S_ISREG (fileinfo.st_mode)) 
    {
        //Do work on just the file
        fileWideRename(argv[3],argv[1],argv[2]);
    }
    else if (S_ISDIR (fileinfo.st_mode)) 
    {
        //Do work on an entire directory and sub directories
        dirWideRename(argv[3],argv[1],argv[2]);
    }
    else
    {
      fprintf(stderr, "Cannot identify %s .\n",argv[3]);
      return;
    }
}

void dirWideRename(char * dirPath, char * toFind, char * replacement)
{
  //Rename directory
  //Make copy of name to modify
  char * newDirPath = Malloc((strlen(dirPath) + 1)*sizeof(char));
  strcpy(newDirPath,dirPath);
  //Run string replace
  strReplaceAll(&newDirPath,toFind,replacement);
  //Rename the file
  Rename(dirPath, newDirPath);
  
  
  //Loop through all files and dirs in directory
  DIR *directory;
  struct dirent *de;
  if (!(directory = opendir(newDirPath)))
  {
      error("Failed to open directory");
  }
  
  while (0 != (de = readdir(directory))) 
  {
      recursiveFileDirProcess(newDirPath,de->d_name, toFind, replacement);
  }
  //Close
  closedir(directory);
  free(newDirPath);
}

void recursiveFileDirProcess(char * dir, char * file,char * toFind, char * replacement )
{
  //Ignore current dir and upper dir
  if(file[0] == '.')
  {
    //Ignored
  }
  else
  {
    //Make full path
    //+2 for /
    char * fullPath = Malloc((strlen(dir) + strlen(file) + 2)*sizeof(char));
    strcpy(fullPath,dir);
    strcat(fullPath,file);
    
    //Decide if this is a file of a directory
    //Check type of input
    //Create stat struct to get info, and int to store status
    int filestatus;
    struct stat fileinfo;
    //Populate struc and status
    filestatus = stat(fullPath,&fileinfo);
    
    if(filestatus !=0)
    {
      fprintf(stderr, "Error in opening %s . Return code: %d\n",fullPath ,filestatus);
      return;
    }
    
    //Check if file or directory
    if (S_ISREG (fileinfo.st_mode)) 
    {
        //Do work on just the file
        fileWideRename(fullPath,toFind,replacement);
    }
    else if (S_ISDIR (fileinfo.st_mode)) 
    {
        //Do work on an entire directory and sub directories
        //Put / on end of string if dir
        strcat(fullPath,"/");
        dirWideRename(fullPath,toFind,replacement);
    }
    else
    {
      fprintf(stderr, "Cannot identify %s .\n",fullPath);
      return;
    }
    free(fullPath);
  }
}


void fileWideRename(char * filePath, char * toFind, char * replacement)
{
    printf("Replacing all occurences in file: %s\n", filePath);
    //Rename the file name first off
    //Make copy of name to modify
    char * newFilePath = Malloc((strlen(filePath) + 1)*sizeof(char));
    strcpy(newFilePath,filePath);
    //Run string replace
    strReplaceAll(&newFilePath,toFind,replacement);
    //Rename the file
    Rename(filePath, newFilePath);
    
    //Replace all strings in the file
    fileReadReplace(newFilePath,toFind,replacement);
    free(newFilePath);
    return;
}

void fileReadReplace(char * filePath, char * toFind, char * replacement)
{
  //This is done by reading from one file while writing to another
  //This is slower than using a cursor (probably) but
  //Is less prone to errors and also offers a bit of protection
  //In case the file read and write process really runs amuck
  
  //Create a tmp file name
  char * tmpFileName = getTmpFileName(filePath);
  if(tmpFileName == NULL)
  {
    fprintf(stderr, "Failed to create tmp file copy for %s .\n",filePath);
    return;
  }
  else
  {
    printf("Created tmp file copy: %s\n",tmpFileName);
  }
 
  //Open the original file for reading
  int readFd = Open(filePath,O_RDONLY,0);
  
  //Open tmp file for writing and creating
  int writeFd = Open(tmpFileName,O_WRONLY | O_CREAT,0644);

  int n; //Number of bytes
  rio_t rio; //RIO type that contains file info
  char buf[MAXLINE]; //Character buffer with max size defined by csapp

  //Associate a descriptor with a read buffer and reset buffer 
  rio_readinitb(&rio,readFd);
  
  int extraChars = 0;
  while((n = rio_readnb(&rio, buf, MAXLINE)) != 0) //While it has read more than zero bytes
  {
    //Run buf through strReplaceAll
    char * bufptr = buf;
    extraChars = strReplaceAll(&bufptr,toFind, replacement);
    //Increase number of bytes to write?
    rio_writen(writeFd, buf, n + (extraChars*sizeof(char)));//Write from buf
  }
  
  Close(writeFd);
  Close(readFd);
  
  //Remove old file, rename tmp to old name
  Remove(filePath);
  Rename(tmpFileName, filePath);
  free(tmpFileName);
}

void Remove(char * filename)
{
  int rv = remove(filename);
  if(rv!=0)
  {
    fprintf(stderr, "Cannot remove %s .\n",filename);
  }
  else
  {
    printf("Removed file %s\n", filename);
  }
}

char * getTmpFileName(char * base)
{
  //Generates a random file name
  //By trying all file names with the pattern
  //base<int> until it finds a file that does not yet exist.
  //This is to avoid making a tmp file that overwrites an
  //existing file. Can be done better but
  //for now this is fine.
  int maxSuffix = 99999;
  int maxSuffixStringLen = 5;
  
  
  int filestatus = 0;
  struct stat fileinfo;
  int suffix = 0;
  //File names go from tmp0 on up
  while( (filestatus ==0) && (suffix <=maxSuffix) )
  {
    char * tmpName = Malloc((strlen(base) + 1 + maxSuffixStringLen )*sizeof(char));
    sprintf(tmpName,"%s%d\0",base,suffix);
    filestatus = stat(tmpName,&fileinfo);
    if(filestatus ==-1)
    {
      //File does not exist
      return tmpName;
    }
    filestatus = 0;
    suffix++;
    free(tmpName);
  }
  return NULL;
}

void Rename(char * old, char * new)
{
  int ret = 0;
  if ( (ret = rename(old,new)) != 0 )
  {
    fprintf(stderr, "Error in renaming %s to %s. Return code: %d\n", old, new, ret);
  }
  else
  {
    printf("Renamed %s to %s\n", old, new);
  }
}

//Double star to modify the char *  (string)
//Returns extra chars added or removed
int strReplaceAll(char ** source, char * toFind, char * replacement)
{
  //The maximum new string size is length of the replacement * ((length of the source / length of tofind) + 1)
  //For ex.  
  //Source:   abababababababa
  //To Find:  ab
  //Rep:      xyz
  //Rep Len: 3
  //Source Len: 15
  //To Find Len: ab
  //(length of the source / length of tofind) = (15 / 2) + 1 = 8
  // 8 * 3 = 24
  
  
  //I feel like if this program has errors
  //It will be in these next memory management lines
  //(or lack of?) Can't get this to work correctly
  //All seems fine for now though
  //Allocate a maximum return string size as described above
  //In case replacement is 0 length, add one
  //char * returnString = Malloc( ((strlen(replacement)+1) * ((strlen(*source)/strlen(toFind))+1))*sizeof(char));
  //*source = realloc(*source, ((strlen(replacement)+1) * ((strlen(*source)/strlen(toFind))+1))*sizeof(char));
  //Copy the source into the return to start
  //strcpy(returnString,*source);
  
  //Keep replacing while the returned index value is appropriate
  int startIndex = 0;
  int change = 0;
  do
  {
    change+= strReplaceOnce(&startIndex, source,toFind,replacement);
  }while((startIndex < strlen(*source)) && (startIndex!=-1) );
  return change;
}

//Checks if toFind is found at a specific index
int sameChars(int sourceindex, char * source, char * toFind)
{
  int i;
  int sourcelen = strlen(source);
  int findlen = strlen(toFind);
  
  //a b c d e f g h i j k
  //0 1 2 3 4 5 6 7 8 9 10
  
  for(i = 0; (i < findlen) && ((sourceindex + i) < sourcelen) ; i++)
  {
    if( toFind[i] != source[sourceindex + i] )
    {
      return 0; 
    }
  }
  return 1;
}

int strReplaceOnce(int * startIndex, char ** source, char * toFind, char * replacement)
{
  int originalLength = strlen(*source);
  int windowSize = strlen(toFind);
  //Calculate additonal space needed if replacement is done
  int shift = strlen(replacement) - strlen(toFind);
  int windowIndex = *startIndex; //Start search where specified
  int replacelen = strlen(replacement);
  //Loop through source string
  for( ; (windowIndex + windowSize) < originalLength ; windowIndex++)
  {
    if(sameChars(windowIndex, *source,toFind))
    {
      //Shift over chars
      shiftChars(source, windowIndex, windowSize, shift);
      //Write new chars
      int i;
      for(i = 0; i < replacelen; i ++)
      {
	(*source)[windowIndex + i] = replacement[i];
      }
      break;
    }
  }
  if((windowIndex + windowSize) >= originalLength)
  {
    //Search has ended
    *startIndex = -1;
  }
  else
  {
    //Change index passed to reflect where left off
    *startIndex = windowIndex + replacelen;
  }
  
  //Return change in size
  int endLength = strlen(*source);
  return endLength - originalLength;
}

void shiftChars(char ** source , int windowIndex, int windowSize, int shamt)
{
  //Positive shift start from end
  if(shamt >=0)
  {
    //Start from end of string
    int i;
    for(i = strlen(*source) - 1; i >= (windowIndex + windowSize) ; i--)
    {
      (*source)[i+shamt] = (*source)[i];
    }
    (*source)[strlen(*source) + shamt] = '\0';
  }
  else
  {
    //Start from end of replace string
    int startIndex = windowIndex + windowSize;
    int lensrc = strlen(*source);

    int i;
    for(i = startIndex; i < lensrc  ; i++)
    {
      (*source)[i+shamt] = (*source)[i];
    }
    (*source)[strlen(*source) + shamt] = '\0';
  }
}

