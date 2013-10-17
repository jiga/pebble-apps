#include<stdio.h>
#include<sys/time.h>

int toss()
{
  long _seed = time(NULL);
  
  printf("\n_seed %l\n", _seed);

  _seed = (((_seed * 214013L + 2531011L) >> 16) & 32767);
  return ((_seed % 2));
}

int main()
{

  printf("\ntoss %d\n", toss());
  return 0;
}

