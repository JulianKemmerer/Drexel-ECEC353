#include <stdio.h>
#include <stdlib.h>
#include "csapp.h"
#include <time.h> //Random
#include <signal.h> //Kill children

//Compile and run with gcc FourInAPipe.c csapp.c -pthread -o FourInAPipe && ./FourInAPipe

#define GRID_N  5 // NxN grid exists
#define M_CHILDREN  20 // Number of children games to spawn
#define EMPTY_SPACE 0 //Integer for empty space in board
#define PARENT_PIECE 1 //Integer for parent piece in board
#define CHILD_PIECE 2 //Integer for child piece in board

//These functions work only on the activeBoard
//Since both the parent or child is only handling one board at a time
//No need to pass the board to every function
//Removed initBoard and replaced with initStructBoard

//Place piece
void placePiece(int column,int pieceType);
//Remove peice - remove last peice put in column
void removePiece(int column);
//Check if board if full
int boardFull();
//Check for winner - returns piece type of winner 
//(which has to be of the supplied pieceType), or empty space if none
int getWinner(int pieceType);
//Smart play - intelligently try to win or block child from winning
//Return column played
int smartPlay(int pieceType);
//Dumb play - return column played
int dumbPlay(int pieceType);
//Column empty? - Wrote this after boardFull...oh well
int columnFull(int column);
//Function to call at the end of the game
void gameOver(int winnerType);
//Print the board to the screen
void printBoard();

//Columns will be 'i'
//'j' will be position in column
//Where higher j values are down the board
//'Gravity' favors higher j values
//Made into struct because syntax for pointers to multi dimension arrays are ugly.
struct Board
{
  int slots[GRID_N][GRID_N];
};

//Store all the boards for the parent
struct Board * parentBoardsArray;
//Store the single board for the child
struct Board childBoard;
//Point to an active board (constant for child to childBoard, changing for parent).
struct Board * activeBoard;

//These functions are specific to certain structs
//Iniatialize a set of empty boards for the parent - populate parentBoardsArray
//Also mark all children as alive in here
void initParentBoards(); 
void initStructBoard(struct Board * board); //Iniatialize a single board

//Pipe/Message Stuff
//Keep track of role in the world
int isParent = 1;

//For parent
//Sending end fds/ids
int sendIDs[M_CHILDREN];
//Receiving end fds/ids
int receiveIDs[M_CHILDREN];
//Keep track of children pids
int childrenPIDs[M_CHILDREN];
//Function to get child index from PID
int childIndexFromPID(int pid);
//Function to get child index from active board
int childIndexFromActiveBoard();
//Function that is run by children only after forking
void birth(int parentRead,int parentWrite);
//Wrapper for forking and other stuff
void copulate(int * childPid, int * toChildID, int * fromChildID);
//Init sending and receiving pipes/messages
void initSendReceive(int * childWrite, int * childRead, int * parentWrite, int * parentRead);
//Wait for child to play
void waitForChildMove(int childIndex);
//Have parent make a play and tell child
void parentPlayChild(int childIndex);
//Wait for parent to play
void waitForParentMove();
//Have child play parent and tell
void childPlayParent();
//Send a move to an appropriate fd/id 
void sendMove(int toID, int column);
//Receive move
int readTurn(int readFromID);
//Keep track of alive (game playing) children
int childAlive[M_CHILDREN];
//Check if all children are dead - reap too
int allChildrenDead();

//For child
//Keep track of parent
int sendID=0;
int receiveID=0;
int parentPID=0;

int main(int argc, char** argv)
{
  //Iniatialize random
  srand(time(NULL));
  
  //Initialize all parent boards
  initParentBoards();
  
  //Initialize child board
  initStructBoard(&childBoard);
  
  //Tmps for function call
  int tmpChildPID;
  int tmpToChildID;
  int tmpFromChildID;
  //Child counter
  int i;
  //Create children
  for(i=0; i< M_CHILDREN; i++)
  {
    //Children end up calling birth() and run from there
    //Parent returns from copulate
    copulate(&tmpChildPID,&tmpToChildID,&tmpFromChildID);
    //Kept track of new child
    sendIDs[i] = tmpToChildID;
    receiveIDs[i] = tmpFromChildID;
    childrenPIDs[i] = tmpChildPID;
  }
  
  //Children have been born and are running
  //Now parent needs to do its thing
  //Parent is always first move
  //Make first move for all children
  
  //Keep making moves followed by waiting for
  //child to make move - gameOver is called
  //and proper action will be taken from there
  while(1)
  {
    if(allChildrenDead())
    {
      //Parents turn to die
      exit(0);
    }
    
    int i;
    //Make a move for each child
    for(i=0; i< M_CHILDREN; i++)
    {
      if(childAlive[i])
      {
	parentPlayChild(i);
      }
    }
    
    if(allChildrenDead())
    {
      //Parents turn to die
      exit(0);
    }
    
    //Wait for a move from each child
    for(i=0; i< M_CHILDREN; i++)
    {
      if(childAlive[i])
      {
	waitForChildMove(i);
      }
    }
    
    if(allChildrenDead())
    {
      //Parents turn to die
      exit(0);
    }
  }
  return 0;
}

int allChildrenDead()
{
  //Also reap children here
  pid_t pid;
   int status;
   while((pid = waitpid(-1, &status, WNOHANG)) > 0){}
  
  int i;
  for(i=0; i< M_CHILDREN; i++)
  {

    if(childAlive[i])
    {
      return 0;
    }
    else
    {
    }
  }
  return 1;
  
}

int childIndexFromActiveBoard()
{
  //Loop through boards
  //Look for active board
  //Return index
  int i;
  for(i=0; i< M_CHILDREN; i++)
  {
    if(&(parentBoardsArray[i]) == activeBoard)
    {
      return i;
    }
  }
  return -1;
}

int readTurn(readFromID)
{ 
  //Pipe reading
  
  //Get move from reading
  int n;
  int columnRead;
  if((n=read(readFromID, &columnRead, sizeof(int))) < 0)
  {
    //Error
    return -1;
  }
  else
  {    
    return columnRead;
  }
}

void waitForChildMove(int childIndex)
{ 
  //Get move from reading
  int columnRead = readTurn(receiveIDs[childIndex]);
  
  //Make this move on parent board
  //Set the child board as the active board
  activeBoard = &parentBoardsArray[childIndex];
  
  placePiece(columnRead, CHILD_PIECE);
  
  //Check for full and win
  if(getWinner(CHILD_PIECE) == CHILD_PIECE)
  {
    //Child won
    gameOver(CHILD_PIECE);
  }
  else if(boardFull())
  {
    gameOver(EMPTY_SPACE);
  } 
}


void parentPlayChild(int childIndex)
{
  //Set the child board as the active board
  activeBoard = &parentBoardsArray[childIndex];

  //smart and dumb play return column played unless game is won
  //If won, game over is called, handle form there
  //Otherwise notify child of parent play
  int columnPlayed = dumbPlay(PARENT_PIECE);
  
  //Notify child of column played
  sendMove(sendIDs[childIndex],columnPlayed);
}

void sendMove(int toID, int column)
{   
  //Send a message appropriately
  if(write(toID, &column , sizeof(int)) != sizeof(int))
  {
       printf("Write pipe error.\n");
  }
}

void copulate(int * childPid, int * toChildID, int * fromChildID)
{
  //Send receive info
  int toChildWrite, fromChildRead, toParentWrite,fromParentRead;
  
  //Init the sending and receiving
  initSendReceive(&toChildWrite,&fromChildRead, &toParentWrite, &fromParentRead);
  
  //Perform the forking!
  int pid = fork();
  
  if(pid == 0) 
  { 
    //A new child is born!
    //Supply child with information to
    //Communicate with parent
    //And begin life
    isParent = 0;
    //(int readFromParent,int writeToParent)
    birth(fromParentRead,toParentWrite);
    //Should never return from birth
    //But just in case, die
    //Poetic right?
    printf("Error: child returned from birth function\n");
    exit(-1);
  }
  else
  {
    //Parent
    isParent = 1;
    //Record the info for the child
    *childPid = pid;
    *toChildID = toChildWrite;
    *fromChildID = fromChildRead;
    //Then just return as normal
  }
}

void initSendReceive(int * toChildWrite, int * fromChildRead, int * toParentWrite, int * fromParentRead)
{
  //Tmp store pipefds
  int parenttochild[2];
  int childtoparent[2];
  //Init pipes
  if(pipe(parenttochild) < 0)
  {
    printf("Parent to child pipe creation error.\n");
  }
  if(pipe(childtoparent) < 0)
  {
    printf("Child to parent pipe creation error.\n");
  }
  
  //'Return' the values
  *toChildWrite = parenttochild[1];
  *fromChildRead = childtoparent[0];
  *toParentWrite = childtoparent[1];
  *fromParentRead = parenttochild[0];
}


void birth(int readFromParent,int writeToParent)
{
  //Store this info globally
  sendID=writeToParent;
  receiveID=readFromParent;
  parentPID=getppid();
  isParent = 0;
  //Set active board (constant)
  activeBoard = &childBoard;
  
  //Wait for parent to make first move
  //Loop waiting for parent then making own more
  while(1)
  {
    waitForParentMove();
    childPlayParent();
  }
}

void waitForParentMove()
{
  //Get move from reading
  int columnRead = readTurn(receiveID);
  
  //Make this move on child board 
  placePiece(columnRead, PARENT_PIECE);
  
  //Child does not check for winning
}

void childPlayParent()
{  
  //Child does dumbplay
  //Child does not check for winning
  int columnPlayed = smartPlay(CHILD_PIECE);

  //Notify parent of column played
  sendMove(sendID,columnPlayed);
}

int childIndexFromPID(int pid)
{
  //Loop through childrenPIDs[M_CHILDREN]
  //Look for supplied pid
  //Return index
  int i;
  for(i=0; i< M_CHILDREN; i++)
  {
    if(childrenPIDs[i] == pid)
    {
      return i;
    }
  }
  return -1;
}

void initParentBoards()
{
  //Also mark all children as alive in here
  
  parentBoardsArray = Malloc(M_CHILDREN*sizeof(struct Board));
  
  int m;
  for(m=0; m< M_CHILDREN; m++)
  {
    initStructBoard(&parentBoardsArray[m]);
    childAlive[m] = 1;
  }
}

void initStructBoard(struct Board * b)
{
  //Run through matrix and set all zeros
  int i,j;
  for(i=0; i< GRID_N;i++)
  {
    for(j=0; j< GRID_N;j++)
    {
     b->slots[i][j]=EMPTY_SPACE; 
    }
  }
}

//These functions below do not have a specific board passed to them but depend on the active board.

void printBoard()
{
  //Run through matrix and set all zeros
  int i,j;
  for(j=0; j< GRID_N;j++)
  {
   for(i=0; i< GRID_N;i++)
   {
     printf("%d ",activeBoard->slots[i][j]); //Row of ints
   }
   //Next line
   printf("\n");
  }
}

void placePiece(int column, int pieceType)
{
  if((column >=0) && (column <= GRID_N-1))
  {
    //Column number makes sense
    //Find highest index where value is 0
    //Loop from bottom of board (higher j)
    int j;
    for(j=GRID_N-1; j>=0;j--)
    {
      if(activeBoard->slots[column][j]==EMPTY_SPACE)
      {
	//Place piece here
	activeBoard->slots[column][j]=pieceType;
	break;
      }
    }
  }
  else
  {
    //Do nothing
    //printf("Sweet jesus! Column %d doesn't exist!\n",column);
  }
}

int boardFull()
{
  //Run through board and look for empty spaces
  int i,j;
  for(i=0; i< GRID_N;i++)
  {
    for(j=0; j< GRID_N;j++)
    {
      if(activeBoard->slots[i][j]==EMPTY_SPACE)
      {
	return 0;
      }
    }
  }
  return 1;
}

//Check for winner - returns piece type of winner 
//(which has to be of the supplied pieceType), or empty space if none
int getWinner(int pieceType)
{
  //Iterators
  int i,j;
   
  //Check all winning vertical positions
  //Vertical winning is defined by base position i,j
  //And other positions:
  //i,j+1
  //i,j+2
  //i,j+3
  for(i=0; i< GRID_N;i++)
  {
    for(j=0; j+3< GRID_N;j++)
    {
      if( (activeBoard->slots[i][j]==pieceType) && (activeBoard->slots[i][j+1]==pieceType) && (activeBoard->slots[i][j+2]==pieceType) && (activeBoard->slots[i][j+3]==pieceType))
      {
	//Win found
	return pieceType;
      }
    }
  }
  
  
  //Check all winning horozonal positions
  //Horozontal winning is defined by base position i,j
  //And other positions:
  //i+1,j
  //i+2,j
  //i+3,j
  for(i=0; i+3< GRID_N;i++)
  {
    for(j=0; j< GRID_N;j++)
    {
      if( (activeBoard->slots[i][j]==pieceType) && (activeBoard->slots[i+1][j]==pieceType) && (activeBoard->slots[i+2][j]==pieceType) && (activeBoard->slots[i+3][j]==pieceType))
      {
	//Win found
	return pieceType;
      }
    }
  }
  
  //Check all diagonals
  //Diagonal winning is defined 
  //i,j
  //i+1,j+1
  //i+2,j+2
  //i+2,j+3
  //OR
  //i,j+3
  //i+1,j+2
  //i+2,j+1
  //i+3,j
  for(i=0; i+3< GRID_N;i++)
  {
    for(j=0; j+3< GRID_N;j++)
    {
      if( 
	
	(
	(activeBoard->slots[i][j]==pieceType) && (activeBoard->slots[i+1][j+1]==pieceType) && (activeBoard->slots[i+2][j+2]==pieceType) && (activeBoard->slots[i+3][j+3]==pieceType)
	)
	||
	(
	(activeBoard->slots[i][j+3]==pieceType) && (activeBoard->slots[i+1][j+2]==pieceType) && (activeBoard->slots[i+2][j+1]==pieceType) && (activeBoard->slots[i+3][j]==pieceType)
	)
	
	)
      {
	//Win found
	return pieceType;
      }
    }
  }
  
  //No ones 
  return EMPTY_SPACE;
}

//Remove peice - remove last peice put in column
void removePiece(int column)
{
  if((column >=0) && (column <= GRID_N-1))
  {
    //Column number makes sense
    //Find highest index where value is 0
    //Loop from top of board (low j)
    int j;
    for(j=0; j< GRID_N;j++)
    {
      if(activeBoard->slots[column][j]!=EMPTY_SPACE)
      {
	//Make that space empty
	activeBoard->slots[column][j]=EMPTY_SPACE;
	return;
      }
    }
    printf("Sweet jesus! Column %d was empty already!\n",column);
  }
  else
  {
    printf("Sweet jesus! Column %d doesn't exist!\n",column);
  }
}

//Smart play - intelligently try to win or block child from winning
//Return the colummn that was played
int smartPlay(int pieceType)
{
  //Trying placing piece in every column, checking for win, remove if not a win
  int i;
  for(i=0; i< GRID_N; i++)
  {
    if(columnFull(i)==0)
    {
      //Column is not full
      //Place peice
      placePiece(i,pieceType);
      //Check for win only if parent
      if(getWinner(pieceType) == pieceType)
      {
	//If win, move is done, game is over
	//Call game over function
	if(isParent)
	{
	  gameOver(pieceType);
	}
	else
	{
	  return i;
	}
      }
      else
      {
	//If not win, remove peice - go to next iteration
	removePiece(i);
      }
    }
  }
  
  //Trying placing piece in every column, checking for opponent win - block this by placing own piece there
  int opponentType;
  if(pieceType == PARENT_PIECE)
  {
    opponentType = CHILD_PIECE;
  }
  else if(pieceType == CHILD_PIECE)
  {
    opponentType = PARENT_PIECE;
  }
  else
  {
    printf("Sweet jesus! What kind of piece is that!?\n");
  }
  
  for(i=0; i< GRID_N; i++)
  {
    if(columnFull(i)==0)
    {
      //Column is not full
      //Place opponent peice
      placePiece(i,opponentType);
      //Check for win by opponent
      if(getWinner(opponentType) == opponentType)
      {
	//If win, block this move by
	//Remove opponent peice
	removePiece(i);
	//Place your peice
	placePiece(i,pieceType);
	return i;
      }
      else
      {
	//If not win, remove opponent peice - go to next iteration
	removePiece(i);
      }
    }
  }
  
  //At this point you could not win and could not block opponent from winning
  //Do a random dumb move
  return dumbPlay(pieceType);
}

//Dumb play - a random move
int dumbPlay(int pieceType)
{
  int randomColumn;
  randomColumn = rand()%GRID_N;//0 to GRID_N -1 number
  
  //End if board is full
  if(boardFull())
  {
    if(isParent)
    {
      gameOver(EMPTY_SPACE);
      return -1;
    }
    else
    {
      return -1;
    }
  }
  
  while(columnFull(randomColumn))
  {
    //That column is full
    //Generate another random one
    //Reseed
    //srand(time(NULL));
    randomColumn = rand()%GRID_N;//0 to GRID_N -1 number
  }
  
  //Got a column that is not full
  //Place piece there
  placePiece(randomColumn,pieceType);
  
  //Check that we did not just randomly win
   if(getWinner(pieceType) == pieceType)
   {
      if(isParent)
      {
	//Call game over function
	gameOver(pieceType);
      }
      else
      {
	 return randomColumn;
      }
   }
   return randomColumn;
}

void gameOver(int winnerType)
{
  int childGameIndex = childIndexFromActiveBoard();
  printf("BOARD %d\n",childGameIndex);
  //Mark child as dead
  childAlive[childGameIndex] = 0;
  //Then kill child also
  int childPID = childrenPIDs[childGameIndex];
  kill(childPID,SIGKILL);
  
  
  if(winnerType == PARENT_PIECE)
  {
    printf("Sweet jesus! The parent won!\n");
  }
  else if(winnerType == CHILD_PIECE)
  {
    printf("Sweet jesus! The child won!\n");
  }
  else
  {
    printf("Sweet jesus! That is called a stalemate!\n");
  }
  
  //Also reap appropriate child here
  
  printBoard();
  printf("\n");
  //exit(0); //Exit entire program
}

//Column empty? - Wrote this after board full...oh well
int columnFull(int column)
{
  if((column >=0) && (column <= GRID_N-1))
  {
    //Column number makes sense
    int j;
    for(j=0; j<GRID_N;j++)
    {
      if(activeBoard->slots[column][j]==EMPTY_SPACE)
      {
	//Found empty space
	return 0;
      }
    }
    //No empty spaces found
    return 1;
  }
  else
  {
    printf("Sweet jesus! Column %d doesn't exist!\n",column);
  }
}