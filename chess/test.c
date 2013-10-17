#include <stdio.h>
#include "umax4_8.h"

int input[9];

char *board;

static char strBoard[121];

void print_board(void)
{
 int N=-1,P=-1;
 while(++N<121)
   strBoard[++P] = N&8&&(N+=7)?10:".?+nkbrq?*?NKBRQ"[board[N]&15]; /* print board */
 strBoard[121] = '\0';

 printf("%s\n",strBoard);
}


int main()
{
  board = chess_init();
  print_board();
  //int myinput[] = {'a','2','a','4',10};
  //chess_move(myinput);
  //print_board();
  input[0] = 10;
  chess_move(input);
  print_board();
  chess_move(input);
  print_board();
  chess_move(input);
  print_board();
  chess_move(input);
  print_board();

  return 0;
}
