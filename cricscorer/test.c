#include <stdio.h>
#include "scorer.h"

int getKeyIndex(char input)
{
  int i;
  for(i=0; i< sizeof(keyPad); i++) {
    if (keyPad[i] == input) return i;
  }
  return 0;
}

int main(void)
{
    unsigned char input[] = ".B.B1B.B2BW1BOB1B4N6BOB.B1B2BW1N2BNUU.B2BI1B2B4BI6BI.B2BI4BUUUUUUUUUUU";
    int i;
    ScoreBoard* current; 

    current = getScore(0);

    for( i = 0; i < sizeof(input)-1; i++ ) {
        printf("event : %d [%c]\n", getKeyIndex(input[i]), input[i]);
        score(getKeyIndex(input[i]));
    }
    printf("\n ScoreBoard\n\tinnings: 0\n\tscore: %d/%d (%d.%d)\n\tballs: %d\n"
	, current->runs
	, current->wickets
	, current->balls/6
	, (current->balls - (6*(current->balls/6)))
        , current->balls);
    current = getScore(1);
    printf("\n ScoreBoard\n\tinnings: 1\n\tscore: %d/%d (%d.%d)\n\tballs: %d\n"
	, current->runs
	, current->wickets
	, current->balls/6
	, (current->balls - (6*(current->balls/6)))
        , current->balls);

  return 0;
}

