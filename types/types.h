#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>

typedef enum _ansii_esc {
  _ansii_esc_RESET,
  _ansii_esc_CLEAR,
  _ansii_esc_RED,
  _ansii_esc_GREEN,
  _ansii_esc_YELLOW,
  _ansii_esc_BLUE,
  _ansii_esc_PURPLE,
  _ansii_esc_CYAN,
  _ansii_esc_WHITE
} _ansii_esc;

typedef enum chtype { NIL = 0, KING, QUEEN, BISHOP, KNIGHT, ROOK, PAWN } chtype;
typedef enum chcolor { BLACK, WHITE } chcolor;

typedef struct point {
  int x;
  int y;
} point;

typedef struct chpiece {
  chtype type;
  chcolor color;
  int *mov;
} chpiece;

typedef struct chstate {
  bool msgocc;
  bool errocc;
  bool fatalerrocc;
  bool exit;
  char *msg;
  char *errmsg;
  bool fromset;
  bool toset;
  point *from;
  point *to;
  chcolor player;
  _ansii_esc _ansii_colbl;
  _ansii_esc _ansii_colwt;
  _ansii_esc _ansii_colmov;
  _ansii_esc _ansii_colhightlt;
  chpiece **board;
} chstate;

typedef enum _divpos { _divpos_TOP, _divpos_MID, _divpos_BOTTOM } _divpos;

#endif
