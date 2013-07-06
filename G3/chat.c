#include <stdio.h>
#include <stdlib.h>
#include "../csapp.h"

//Compile and run: gcc chat.c ../csapp.c -pthread -o chat && ./chat -s 12345

void printusage();

pthread_t readThread;
pthread_t writeThread;


void * readThreadProc(void * arg)
{
  //All that is passed is fd int
  int fd = *(int*)arg;
  
  //Initialized reliable stuff
  rio_t rio;
  Rio_readinitb(&rio, fd);
  char buf[MAXLINE];
  fflush(stdout);
  //while (Fgets(buf, MAXLINE, fd) != NULL)
  while(1)
  {
    //Receive line
    printf("\nincoming:> ");
    Rio_readlineb(&rio, buf, MAXLINE);
    Fputs(buf, stdout); 
  }
  pthread_exit(NULL);
}

void * writeThreadProc(void * arg)
{
  //All that is passed is fd int
  int fd = *(int*)arg;
  //Initialized reliable stuff
  rio_t rio;
  char buf[MAXLINE];
  //Print prompt
  printf("outgoing:> "); fflush(stdout);
  while (Fgets(buf, MAXLINE, stdin) != NULL) 
  {	
    //Send line
    Rio_writen(fd, buf, strlen(buf));
    printf("outgoing:> "); fflush(stdout);
  }
  pthread_exit(NULL);
}


int main(int argc, char** argv)
{
  if(argc <3)
  {
    printusage();
  }
  else
  {
    // argv[1] is flag
    if(argv[1][1] == 'c')
    {
      printf("Starting Chat as Client\n");
      char * serverhost = argv[2];
      int serverport = atoi(argv[3]);
      printf("Trying to connect to %s on port %d ...\n", serverhost,serverport);
      //Connect to server
      //Get fd
      int clientfd = Open_clientfd(serverhost, serverport);
      printf("Client connected to server!\n");
      //Spawn thread to handle writing out to server
      int rc = pthread_create(&writeThread, NULL, writeThreadProc, (void *)&clientfd);
      if (rc){
         printf("ERROR: return code from pthread_create() is %d\n", rc);
         exit(-1);
      }
            
      //Spawn thread to handle reading from server
      rc = pthread_create(&readThread, NULL,readThreadProc, (void *)&clientfd);
      if (rc){
         printf("ERROR: return code from pthread_create() is %d\n", rc);
         exit(-1);
      }      
    }
    else if(argv[1][1] == 's')
    {
      printf("Starting Chat as Server\n");
      int listenport = atoi(argv[2]);
      printf("Listening on port %d ...\n", listenport);
      
      //Some jazzy variables
      int listenfd, connfd, clientlen;
      struct sockaddr_in clientaddr;
      struct hostent *hp;
      char *haddrp;
     
      //Listen on the listenport
      listenfd = open_listenfd(listenport); 

      //Leave main thread to listen
      while (1) 
      {
	//Some jazz to get a connection
	clientlen = sizeof(clientaddr); 
	connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
	/*hp = Gethostbyaddr(
			(const char *)&clientaddr.sin_addr.s_addr,
			sizeof(clientaddr.sin_addr.s_addr), 
			AF_INET
			 );
	haddrp = inet_ntoa(clientaddr.sin_addr);
	printf("Server connected to %s (%s)\n", hp->h_name, haddrp);
	*/
	printf("Server got connection!\n");

	//Spawn new thread to read and write to accepted connection
	int rc = pthread_create(&writeThread, NULL, writeThreadProc, (void *)&connfd);
	if (rc){
	  printf("ERROR: return code from pthread_create() is %d\n", rc);
	  exit(-1);
	}
	      
	//Spawn thread to handle reading from server
	rc = pthread_create(&readThread, NULL,readThreadProc, (void *)&connfd);
	if (rc){
	  printf("ERROR: return code from pthread_create() is %d\n", rc);
	  exit(-1);
	}  
      }
    }
    else
    {
      printf("Unrecognized flag: %c\n", argv[1][1]);
      printusage();
    }
  }
  /* Last thing that main() should do */
  pthread_exit(NULL);
}

void printusage()
{
  printf("Must supply flags for server or client\n");
  printf("Client:\n");
  printf("$ ./chat -c <server ip> <port>:\n");
  printf("Server:\n");
  printf("$ ./chat -s <port>:\n");
}

