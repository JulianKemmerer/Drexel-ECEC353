#include "../csapp.h"
//Compile with $ gcc -pthread writer.c ../csapp.c -o writer
//Read from stdin and write to file

int main(int argc, char **argv) 
{   
    if(!(argc >=2) ) //If commandline args are not provided
    {
      printf("No file provided\n");
      return -1;
    }
    
    //Use csapp Open to get fd number (creates files)
    int fd = Open(argv[1],O_CREAT | O_WRONLY,0);
  
    int n; //Number of bytes
    rio_t rio; //RIO type that contains file info
    char buf[MAXLINE]; //Character buffer with max size defined by csapp

    //Associate a descriptor with a read buffer and reset buffer 
    //the POSIX <unistd.h> definition is STDIN_FILENO
    Rio_readinitb(&rio,STDIN_FILENO); 
    
    while((n = rio_readnb(&rio, buf, MAXLINE)) != 0) //While it has read more than zero bytes
    {
	Rio_writen(fd, buf, n);//Write n bytes from buf
    }
    
    Close(fd);
    
    exit(0);
}