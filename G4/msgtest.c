#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

#define	BUFFSIZE	128
#define	PERMS		0666
#define KEY ((key_t) 7777)

int main(int argc, char** argv)
{
  //New queue id
  int	msqid;
  //Structs to hold send and receive info
  struct {
    long  m_type;
    char  m_text[BUFFSIZE];
  } msgbuffs, msgbuffr;
  
  //Try to open a queue
  if ( (msqid = msgget(KEY, PERMS | IPC_CREAT)) < 0)
  {
    perror("msgget error");
  }
  
  //Set message type
  msgbuffs.m_type = 1L;
   
  //Copy in message
  strcpy(msgbuffs.m_text,"a REALLY boring message");
   
  //Send the message
  if (msgsnd(msqid, &msgbuffs, BUFFSIZE, 0) < 0)
  {
    perror("msgsnd error");
  }

  //Print sent message
  printf("the message sent is: %s \n", msgbuffs.m_text); 

  //Receive the message
  if (msgrcv(msqid, &msgbuffr, BUFFSIZE, 0L, 0) != BUFFSIZE)
  {
    perror("msgrcv error");
  }

  //Print received message
   printf("the message received is: %s \n", msgbuffr.m_text);

   //Remove msg
    if (msgctl(msqid, IPC_RMID, (struct msqid_ds *) 0) < 0)
    {
	perror("IPC_RMID error");
    }
}
