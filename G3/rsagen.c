#include "rsa.h"
#include <stdio.h>
#include <stdlib.h>


//Compile and run with gcc rsagen.c rsa.c -o rsagen  && ./rsagen

int main(int argc, char** argv)
{
  int nth = 1;
  int mth = 1;
  long c = 0;
  long a = 0;
  long b = 0;
  long m = 0;
  long e = 0;
  long d = 0;
  
  printf("Enter the nth and mth prime to compute: <n><space><m>\n");
  scanf("%d %d",&nth,&mth);
  
  if(nth ==mth)
  {
    printf("Wow. Great crypto here. nth == mth. Bye.\n");
    return -1;
  }
  
  
  a = nthprime(nth);
  b = nthprime(mth);
  printf("primes[%d] = %ld\nprimes[%d] = %ld\n ",nth,a,mth,b);
  printf("\n");
  
  m=(a-1)*(b-1);
  e=coprime(m);
  //e, m
  d=mod_inverse(e,m);
  //Example
  //e = 451
  //m = 2520
  //d = 1531
  c=a*b;
  printf("c=%ld\n",c);
  printf("m=%ld\n",m);
  printf("e=%ld\n",e);
  printf("d=%ld\n",d);
  
  printf("Public Key:\n(%ld,%ld)\n",e,c);
  printf("Private Key:\n(%ld,%ld)\n",d,c);
  
  //Test
  /*
  
  printf("\n\n\nTEST\n");
  
  int i;
  char str[6] = "Hello";
  for(i=0; i<=5;i++)
  {
    printf("%c=%ld\n",str[i],endecrypt(str[i], 451, 2623));
  }
  */
      
  return 0;
}