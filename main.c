#include "lib/utils.h"
#include "tui/table.h"
#include "types/types.h"
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

void resetmov(int *mov) {
  for (int i = 0; i < 64; i++) {
    mov[i] = 0;
  }
}

void render_piece(chstate *state, int row, int col, bool highlight) {
  const int i = get_board_index(row, col);
  const wchar_t chlookup[] = {
      [KING] = L'♛', [QUEEN] = L'♚', [BISHOP] = L'♝', [KNIGHT] = L'♞',
      [ROOK] = L'♜', [PAWN] = L'♟',  [NIL] = ' '};
  printansii(state->board[i]->color == BLACK ? state->_ansii_colbl
                                             : state->_ansii_colwt);
  if (highlight == true) {
    if (state->board[i]->type == NIL) {
      printansii(state->_ansii_colmov);
      wprintf(L"%lc", L'•');
      printansii(_ansii_esc_RESET);
      return;
    }
    printansii(state->_ansii_colhightlt);
  }
  wprintf(L"%lc", chlookup[state->board[i]->type]);
  printansii(_ansii_esc_RESET);
}

void render_logs(chstate *state) {
  if (state->msgocc == true) {
    wprintf(L"%s\n", state->msg);
  }
  if (state->errocc == true) {
    wprintf(L"ERR: %s\n", state->errmsg);
  }
  state->msgocc = false;
  state->errocc = false;
}

void prompt(chstate *state) {
  char *pl = state->player == BLACK ? "black" : "white";
  char *turn = state->fromset == false ? "from" : "to";
  wprintf(L"%s:%s> ", pl, turn);
}

int parsecmd(chstate *state, char *line, int len) {
  if (line[1] == 'q') {
    state->exit = true;
    return 1;
  }
  return 0;
}

int parsemov(chstate *state, char *line, int len, point *p) {
  if (len != 3) {
    return 0;
  }
  if (line[0] < 'a' || line[0] > 'h' || line[1] < '1' || line[1] > '8') {
    return 0;
  }
  p->y = line[0] - 'a';
  p->x = 7 - (line[1] - '1');
  return 1;
}

bool checkmov(chstate *state) {
  int from = get_board_indexp(state->from);
  int to = get_board_indexp(state->to);
  return state->board[from]->mov[to] == 1;
}

void newmovpawn(chstate *state, int prow, int pcol) {
  chpiece *p = state->board[get_board_index(prow, pcol)];
  const int r1 = prow + (p->color == BLACK ? -1 : 1);

  if (isbound(r1, pcol) && !isocc(state, r1, pcol)) {
    p->mov[8 * r1 + pcol] = 1;
  }

  const int c1 = pcol + 1;
  bool isdiagrtvalid = isbound(r1, c1) && isocc(state, r1, c1) &&
                       state->board[get_board_index(r1, c1)]->color !=
                           state->board[get_board_index(prow, pcol)]->color;
  if (isdiagrtvalid) {
    p->mov[8 * r1 + c1] = 1;
  }

  const int c2 = pcol - 1;
  bool isdiaglftvalid = isbound(r1, c2) && isocc(state, r1, c2) &&
                        state->board[get_board_index(r1, c2)]->color !=
                            state->board[get_board_index(prow, pcol)]->color;
  if (isdiaglftvalid) {
    p->mov[8 * r1 + c2] = 1;
  }

  if (prow == 1 || prow == 6) {
    const int r2 = prow + (p->color == BLACK ? -2 : 2);
    if (isbound(r2, pcol) && !isocc(state, r2, pcol)) {
      p->mov[8 * r2 + pcol] = 1;
    }
  }
}

// for ROOK, BISHOP, QUEEN
void newmovrbq(chstate *state, int prow, int pcol) {
  int start = 0;
  int end = 0;
  chpiece *p = state->board[get_board_index(prow, pcol)];

  if (p->type == ROOK) {
    end = 4;
  }

  if (p->type == QUEEN) {
    end = 8;
  }

  if (p->type == BISHOP) {
    start = 4;
    end = 8;
  }

  int dir[8][2] = {{1, 0}, {0, 1},  {-1, 0}, {0, -1},
                   {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

  for (int i = start; i < end; i++) {
    int row = prow;
    int col = pcol;

    while (true) {
      row += dir[i][0];
      col += dir[i][1];
      if (!isbound(row, col)) {
        break;
      }
      if (isocc(state, row, col)) {
        if (state->board[get_board_index(row, col)]->color !=
            state->board[get_board_index(prow, pcol)]->color) {
          state->board[get_board_index(prow, pcol)]
              ->mov[get_board_index(row, col)] = 1;
        }
        break;
      }
      state->board[get_board_index(prow, pcol)]
          ->mov[get_board_index(row, col)] = 1;
    }
  }
}

void newmovknight(chstate *state, int prow, int pcol) {
  int pos[8][2] = {{2, 1},  {2, -1},  {1, 2},  {1, -2},
                   {-2, 1}, {-2, -1}, {-1, 2}, {-1, -2}};
  for (int i = 0; i < 8; i++) {
    int row = prow + pos[i][0];
    int col = pcol + pos[i][1];
    if (!isbound(row, col)) {
      continue;
    }
    if (isocc(state, row, col)) {
      if (state->board[get_board_index(row, col)]->color !=
          state->board[get_board_index(prow, pcol)]->color) {
        state->board[get_board_index(prow, pcol)]
            ->mov[get_board_index(row, col)] = 1;
      }
      continue;
    }
    state->board[get_board_index(prow, pcol)]->mov[get_board_index(row, col)] =
        1;
  }
}
void newmovking(chstate *state, int prow, int pcol) {}

void recompmov(chstate *state, int row, int col) {
  switch (state->board[get_board_index(row, col)]->type) {
  case PAWN:
    return newmovpawn(state, row, col);
  case ROOK:
  case BISHOP:
  case QUEEN:
    return newmovrbq(state, row, col);
  case KING:
    return newmovking(state, row, col);
  case KNIGHT:
    return newmovknight(state, row, col);
  case NIL:
    break;
  }
}

void mov(chstate *state) {
  int from = get_board_indexp(state->from);
  int to = get_board_indexp(state->to);
  state->board[to]->type = state->board[from]->type;
  state->board[to]->color = state->board[from]->color;
  state->board[from]->type = NIL;
  resetmov(state->board[from]->mov);
  resetmov(state->board[to]->mov);
  recompmov(state, state->to->x, state->to->y);

  int diag[8][2] = {{1, 0}, {0, 1},  {-1, 0}, {0, -1},
                    {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
  for (int i = 0; i < 2; i++) {
    point *pt = i == 0 ? state->to : state->from;
    int row = pt->x;
    int col = pt->y;
    for (int j = 0; j < 8; j++) {
      while (true) {
        row += diag[j][0];
        col += diag[j][1];
        if (!isbound(row, col)) {
          break;
        }
        if (!isocc(state, row, col)) {
          continue;
        }
        recompmov(state, row, col);
        break;
      }
    }
  }
}

int readmov(chstate *state) {
  if (state->toset == true) { // handle invalid state
    return 0;
  }
  point *p = state->fromset == false ? state->from : state->to;
  char buf[200];
  char *pl = state->player == BLACK ? "black" : "white";
  char *turn = state->fromset == false ? "from" : "to";
  wprintf(L"%s:%s> ", pl, turn);
  fgets(buf, sizeof(buf), stdin);
  int len = strlen(buf);
  char *line = (char *)malloc(len * sizeof(char));
  strcpy(line, buf);

  int s = parsecmd(state, line, len);
  if (s > 0) {
    return 1;
  }

  s = parsemov(state, line, len, p);
  if (s == 0) {
    p->x = 0;
    p->y = 0;
    return 0;
  }
  if (state->fromset == false) {
    state->fromset = true;
  } else {
    state->toset = true;
  }
  return s;
}

void render(chstate *state) {
  if (state->fatalerrocc == true) {
    return;
  }

  if (state->fromset == false && state->toset == true) {
    state->toset = false;
  }

  if (state->fromset == true && state->toset == true) {
    bool valid = checkmov(state);
    if (valid) {
      mov(state);
      state->player = state->player == BLACK ? WHITE : BLACK;
    }
    state->fromset = false;
    state->toset = false;
    state->from->x = 0;
    state->from->y = 0;
    state->to->x = 0;
    state->to->y = 0;
  }

  printansii(_ansii_esc_CLEAR);
  render_logs(state);

  wtbl *tbl = newchtbl(state);
  wprinttbl(tbl);

  int s = readmov(state);
  if (s == 0) {
    return;
  }

  render(state);
}

void newpiece(chpiece **board, chtype t, chcolor c, int row, int col) {
  int i = get_board_index(row, col);
  board[i]->type = t;
  board[i]->color = c;
}

void fillmovs(chstate *state) {
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      recompmov(state, row, col);
    }
  }
}

chpiece **newboard() {
  chpiece **p = (chpiece **)malloc(64 * sizeof(chpiece *));
  for (int i = 0; i < 64; i++) {
    p[i] = (chpiece *)malloc(sizeof(chpiece));
    int *mov = (int *)calloc(64, sizeof(int));
    for (int i = 0; i < 64; i++) {
      mov[i] = 0;
    }
    p[i]->type = NIL;
    p[i]->color = BLACK;
    p[i]->mov = mov;
  }

  for (int i = 0; i < 8; i++) {
    newpiece(p, PAWN, WHITE, 1, i);
    newpiece(p, PAWN, BLACK, 6, i);
  }

  newpiece(p, ROOK, WHITE, 0, 0);
  newpiece(p, ROOK, WHITE, 0, 7);
  newpiece(p, KNIGHT, WHITE, 0, 1);
  newpiece(p, KNIGHT, WHITE, 0, 6);
  newpiece(p, BISHOP, WHITE, 0, 2);
  newpiece(p, BISHOP, WHITE, 0, 5);
  newpiece(p, KING, WHITE, 0, 3);
  newpiece(p, QUEEN, WHITE, 0, 4);

  newpiece(p, ROOK, BLACK, 7, 0);
  newpiece(p, ROOK, BLACK, 7, 7);
  newpiece(p, KNIGHT, BLACK, 7, 1);
  newpiece(p, KNIGHT, BLACK, 7, 6);
  newpiece(p, BISHOP, BLACK, 7, 2);
  newpiece(p, BISHOP, BLACK, 7, 5);
  newpiece(p, KING, BLACK, 7, 4);
  newpiece(p, QUEEN, BLACK, 7, 3);

  return p;
}

chstate *newstate() {
  chstate *s = (chstate *)malloc(sizeof(chstate));
  s->msgocc = false;
  s->msg = (char *)malloc(200 * sizeof(char));
  s->errocc = false;
  s->errmsg = (char *)malloc(200 * sizeof(char));
  s->fatalerrocc = false;
  s->exit = false;
  s->fromset = false;
  s->toset = false;
  s->from = (point *)malloc(sizeof(point));
  s->to = (point *)malloc(sizeof(point));
  s->from->x = 0;
  s->from->y = 0;
  s->to->x = 0;
  s->to->y = 0;
  s->player = WHITE;
  s->board = newboard();
  s->_ansii_colbl = _ansii_esc_PURPLE;
  s->_ansii_colwt = _ansii_esc_WHITE;
  s->_ansii_colmov = _ansii_esc_GREEN;
  s->_ansii_colhightlt = _ansii_esc_RED;
  fillmovs(s);
  msg(s, "Welcome to the game! Type 'h' for help.");
  return s;
}

int main(int argc, char **argv) {
  enter_alt_screen();
  setlocale(LC_CTYPE, "");
  chstate *state = newstate();
  render(state);
  free(state);
  leave_alt_screen();
  return 0;
}
