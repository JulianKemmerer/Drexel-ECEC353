//Mini RSA functions
#include "rsa.h"

long nthprime(int nth)
{
  if (nth < 1)
  {
    return 0;
  }
  else
  {
    long count = 0;
    long i = 2;
    long rv = 0;
    while(count < nth)
    {
      if( isPrime(i) )
      {
	count++;
      }
      i++;
    }
    return i-1;
  }
}

long coprime(long x)
{
  //If two numbers have GCD=1 then they are coprime
  //Two integers are said to be coprime, if they have no common prime factors.
  //Generate a random prime numbers
  //This does not work if x%prime !=0 because that means
  //X is a multiple of prime, so they are not coprime
  //Keep generating until this is not true
   
  int iseed = (int)time(0);
  srand (iseed);
  int index = rand()%1000;
  long prime = nthprime(index+1); //+1 to never pass 0
  
  while(x%prime ==0)
  {
    //Regenerate a new prime
    iseed = (int)time(0);
    srand (iseed);
    index = rand()%1000;
    prime = nthprime(index+1);
  }
  return prime;
}
long endecrypt(long msg_or_cipher, long key, long c)
{
  return mod_pow(msg_or_cipher,key,c);
}
long GCD(long a, long b)
{
  //Taken from barnyard.syr.edu/quickies/totient.c
  
  long q,r,t;

  /* Normalize so that 0 < a <= b */
  if((a == 0)||(b == 0)) return -1;
  if(a < 0) a = -a;
  if(b < 0) b = -b;
  if(b < a){
	  t = b;
	  b = a;
	  a = t;
  }

  /* Now a <= b and both >= 1. */

  q = b/a;
  r = b - a*q;
  if(r == 0) 
	  return a;

  return GCD(a,r);
}

//Taken from barnyard.syr.edu/quickies/totient.c 
long phiphi(long y,long x);
long totient(long n)
{
	//Taken from barnyard.syr.edu/quickies/totient.c 
	if(n<0)n=-n;
	/* handle a few trivial boundary cases */
	if(n<=1)return 0;
	if(n==2)return 1;
	if(n==3)return 2;
	return phiphi(n,2);
}
//Taken from barnyard.syr.edu/quickies/totient.c 
/* This only gets called with y >= 3 and y > x >= 2 */
long phiphi(long y, long x)
{
	long z;

	if(x+1 == y)return x; /* phi(prime p) = p-1 */
	if((y%x)==0){
		if(GCD(x,z=y/x)==1)
			return totient(x)*totient(z); /* multiplicative property */
		else
			return x*totient(z); /* This is a tricky case. It may
					    happen when x is a prime such
					    that a power of x divides y. In
					    case y = p^n, phi(y) = p^(n-1)(p-1)
					   */
	}
	else return phiphi(y,x+1);
}

long mod_pow(long base,long exp,long mod)
{
  long c = 1;
  long e_prime =1;
  for(e_prime=1; e_prime<= exp; e_prime++)
  {
    c = (c*base)%mod;
  }
  return c;
}

long mod_inverse(long base, long m )
{
  return mod_pow(base,totient(m)-1,m);
}



long modulo(long a, long b, long c)
{
  return mod_pow(a,b,c);
}

int isCoprime(long m, long n)
{
  int gcd = GCD(m,n);
  
  if(gcd==1)
  {
    return 1;
  }
  else
  {
    return 0;
  }
  
}

int isPrime(long n)
{
  //Loop from 1 to half of n (don't both with sqrt)
  int upper = n/2;
  int i;
  int isPrime = 1;
  for(i=2; i<=upper; i++)
  {
    if(n%i == 0)
    {
      isPrime = 0;
      break;
    }
  }
  return isPrime;
}




