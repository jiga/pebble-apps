
#define PIECES ".?+nkbrq?*?NKBRQ"
// printf("%c",N&8&&(N+=7)?10:".?+nkbrq?*?NKBRQ"[b[N]&15]); /* print board */

char *chess_init();

void chess_move(int *input);

