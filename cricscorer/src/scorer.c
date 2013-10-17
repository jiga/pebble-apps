#include "scorer.h"

char keyPad[] = " 1234567.N+WIU";
char *keyLabel[] = {"  0 Run","  1 Run","  2 Runs","  3 Runs","  4 Runs","  5 Runs","  6 Runs","  7 Runs","  Ball","  No Ball","  Wide Ball","  Out!","  Innings","  undo.."};
unsigned char keyHistory[MAX_HISTORY+1] = {NULL_KEY,NULL_KEY,NULL_KEY,NULL_KEY,NULL_KEY,NULL_KEY, NULL_KEY}; //keep last 6 balls history

int keyPos = -1;

ScoreBoard team[MAX_INNINGS];
static ScoreBoard *current = &team[0];
static short innings = 0;

ScoreBoard* getScore(int innings) {
    return &team[innings];
}

void score(int event) {
  int modifier = 1;

  if (event == 13) { // Undo
    POP_KEY(event);
    modifier = -1;
  }
  else if (event != 0 && event != 12) {
    PUSH_KEY(event);
  }

  switch(event) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
        current->runs += modifier * event;
      break;
    case 8:  // Ball
        current->balls += modifier;
      break;
    case 9:  // No-Ball
    case 10: // Wide-Ball
        current->runs += modifier;
      break;
    case 11: // Out
        current->wickets += modifier;
      break;
    case 12: // Innings
      innings ^= 1;
      current = &team[innings];
      break;
    default:
      break;
  }
  if(modifier == -1 && current->runs == 1023)
      current->runs = 0;
  if(modifier == 1 && current->runs > 1023)
      current->runs = 1023;
  if(modifier == -1 && current->balls == 511)
      current->balls = 0;
  if(modifier == 1 && current->balls > 511)
      current->balls = 511;
  if(modifier == -1 && current->wickets == 15)
      current->wickets = 0;
  if(modifier == 1 && current->wickets > 15)
      current->wickets = 15;
}
