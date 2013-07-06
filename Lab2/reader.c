ad#include "../csapp.h"
//Compile with $ gcc -pthread reader.c ../csapp.c -o reader
//Read from file and write to stdout

int main(int argc, char **argv) 
{   
    if(!(argc >=2) ) //If commandline args are not provided
    {
      printf("No file provided\n");
      return -1;
    }
    
    //Use csapp Open to get fd number
    //Not creating file - don't worry about mode
    int fd = Open(argv[1],O_RDONLY,0);
  
    int n; //Number of bytes
    rio_t rio; //RIO type that contains file info
    char buf[MAXLINE]; //Character buffer with max size defined by csapp

    //Associate a descriptor with a read buffer and reset buffer 
    //the POSIX <unistd.h> definition is STDIN_FILENO
    rio_readinitb(&rio,fd); 
    
    while((n = rio_readnb(&rio, buf, MAXLINE)) != 0) //While it has read more than zero bytes
    {
	rio_writen(STDOUT_FILENO, buf, n);//Write n bytes from buf
    }
    
    Close(fd);
    
    exit(0);
}