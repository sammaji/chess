#ifndef TABLE_H
#define TABLE_H

#include "../types/types.h"
#include <wchar.h>

typedef struct wtbl_element {
  _ansii_esc col;
  wchar_t ch;
} wtbl_element;

typedef struct wtbl {
  int rowc;
  int colc;
  bool print_col_left;
  bool print_col_right;
  bool print_row_top;
  bool print_row_bottom;
  wtbl_element **index_col_left;
  wtbl_element **index_col_right;
  wtbl_element **index_row_top;
  wtbl_element **index_row_bottom;
  wtbl_element **el;
} wtbl;

typedef enum wtbl_divpos {
  wtbl_divpos_TOP,
  wtbl_divpos_MIDDLE,
  wtbl_divpos_BOTTOM
} wtbl_divpos;

wtbl *newchtbl(chstate *state);

void wprintonlyif(wchar_t *str, bool cond);
void wprintdiv(wtbl *wtbl, wtbl_divpos pos, int colc);
void wprinttbl(wtbl *wtbl);

#endif
