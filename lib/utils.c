#include "utils.h"

void printansii(_ansii_esc esc) {
  char *ansii_colors[] = {
      [_ansii_esc_RESET] = "\033[0m",     [_ansii_esc_CLEAR] = "\033[H\033[J",
      [_ansii_esc_RED] = "\033[0;31m",    [_ansii_esc_GREEN] = "\033[0;32m",
      [_ansii_esc_YELLOW] = "\033[0;33m", [_ansii_esc_BLUE] = "\033[0;34m",
      [_ansii_esc_PURPLE] = "\033[0;35m", [_ansii_esc_CYAN] = "\033[0;36m",
      [_ansii_esc_WHITE] = "\033[0;37m"};
  wprintf(L"%s", ansii_colors[esc]);
}

int get_board_indexp(point *pt) { return 8 * pt->x + pt->y; }
int get_board_index(int row, int col) { return 8 * row + col; }

bool isbound(int row, int col) {
  return row < 8 && col < 8 && row >= 0 && col >= 0;
}

bool isboundp(point *p) {
  return p->x < 8 && p->y < 8 && p->x >= 0 && p->y >= 0;
}

bool isocc(chstate *state, int row, int col) {
  return state->board[get_board_index(row, col)]->type != NIL;
}

bool isoccp(chstate *state, point *pt) {
  return state->board[get_board_indexp(pt)]->type != NIL;
}

void msg(chstate *state, char *msg) {
  if (state->msgocc == false) {
    state->msgocc = true;
  }
  strcpy(state->msg, msg);
}

void err(chstate *state, char *errmsg) {
  if (state->errocc == false) {
    state->errocc = true;
  }
  strcpy(state->errmsg, errmsg);
}
