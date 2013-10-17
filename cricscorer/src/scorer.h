#define MAX_HISTORY	12	
#define MAX_INNINGS	2
#define NULL_KEY	0

extern char keyPad[14];
extern char *keyLabel[14];
extern unsigned char keyHistory[MAX_HISTORY+1]; //keep last 12 events history

extern int keyPos;

#define PUSH_KEY(k)  {\
  keyHistory[(++keyPos)%MAX_HISTORY] = k;\
}

#define POP_KEY(value) {\
  if(keyPos >= 0) {\
    value = keyHistory[keyPos%MAX_HISTORY];\
    keyHistory[keyPos%MAX_HISTORY] = NULL_KEY;\
    keyPos--;\
  }\
  else\
    value = NULL_KEY;\
}

typedef struct _ScoreBoard {
  volatile unsigned short   inning:2;
  volatile unsigned short   runs:10;
  volatile unsigned short   wickets:4;
  volatile unsigned short   balls:9;
}ScoreBoard;


void score(int event);

ScoreBoard* getScore(int innings); 
