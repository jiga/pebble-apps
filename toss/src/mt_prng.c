/* Mersenne Twister Pseudo Random Number Generator */
/* http://en.wikipedia.org/wiki/Mersenne_twister */

#define LENGTH 624
#define BITMASK_32 0xffffffff
#define BITPOW_31 ((unsigned int)(1<<31))

static unsigned int mt[LENGTH];
static unsigned int idx=0;

void mt_random_init(unsigned int seed)
{
  unsigned int i;
  mt[0]=seed;
  for(i=1;i<LENGTH;i++) mt[i]=(1812433253*(mt[i-1]^(mt[i-1]>>30))+i)&BITMASK_32;

  for(i=0;i<LENGTH;i++){
	unsigned int y=(mt[i]&BITPOW_31)+(mt[(i+1)%LENGTH]&(BITPOW_31-1));
	mt[i]=mt[(i+397)%LENGTH]^(y>>1);
	if(y%2) mt[i]^=2567483615;
  }
}

unsigned int mt_random(unsigned int max)
{
  unsigned int y;
  y=mt[idx];
  y^= y>>11;
  y^=(y<< 7)&2636928640;
  y^=(y<<15)&4022730752;
  y^= y>>18;
  idx=(idx+1)%LENGTH;

  return y%max;
}

