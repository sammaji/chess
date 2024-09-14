#include "table.h"
#include "../lib/utils.h"
#include <stdlib.h>

wtbl *newchtbl(chstate *state) {
  const wchar_t chlookup[] = {
      [KING] = L'♛', [QUEEN] = L'♚', [BISHOP] = L'♝', [KNIGHT] = L'♞',
      [ROOK] = L'♜', [PAWN] = L'♟',  [NIL] = ' ',
  };

  wtbl *tbl = (wtbl *)malloc(sizeof(wtbl));
  tbl->rowc = 8;
  tbl->colc = 8;

  wtbl_element **el = (wtbl_element **)malloc(64 * sizeof(wtbl_element *));
  wtbl_element **index_row =
      (wtbl_element **)malloc(8 * sizeof(wtbl_element *));
  wtbl_element **index_col =
      (wtbl_element **)malloc(8 * sizeof(wtbl_element *));

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      const int index = get_board_index(i, j);
      wtbl_element *element = (wtbl_element *)malloc(sizeof(wtbl_element));
      element->ch = chlookup[state->board[index]->type];
      element->col = state->board[index]->color == WHITE ? _ansii_esc_WHITE
                                                         : _ansii_esc_PURPLE;
      if (state->fromset == true && state->toset == false &&
          (state->board[get_board_indexp(state->from)]->mov[index] == 1)) {
        if (state->board[index]->type == NIL) {
          element->ch = L'•';
          element->col = _ansii_esc_GREEN;
        } else {
          element->col = _ansii_esc_RED;
        }
      }
      el[index] = element;
    }
  }

  tbl->el = el;

  for (int i = 0; i < 8; i++) {
    wtbl_element *rowi = (wtbl_element *)malloc(sizeof(wtbl_element));
    rowi->ch = (wchar_t)(i + 'a');
    rowi->col = _ansii_esc_WHITE;
    index_row[i] = rowi;

    wtbl_element *coli = (wtbl_element *)malloc(sizeof(wtbl_element));
    coli->ch = (wchar_t)(8 - i + '0');
    coli->col = _ansii_esc_WHITE;
    index_col[i] = coli;
  }

  tbl->index_row_top = index_row;
  tbl->index_row_bottom = index_row;
  tbl->index_col_left = index_col;
  tbl->index_col_right = index_col;
  tbl->print_col_left = true;
  tbl->print_col_right = true;
  tbl->print_row_top = true;
  tbl->print_row_bottom = true;
  return tbl;
}

void wprintonlyif(wchar_t *str, bool cond) {
  if (!cond) {
    return;
  }
  wprintf(str);
}

void wprintdiv(wtbl *wtbl, wtbl_divpos pos, int colc) {
  const wchar_t divlookup[] = {L'┌', L'┬', L'┐', L'├', L'┼',
                               L'┤', L'└', L'┴', L'┘'};
  wprintonlyif(L" ", wtbl->print_col_left);
  wprintf(L" %lc", divlookup[3 * pos]);
  for (int i = 0; i < colc - 1; i++) {
    wprintf(L"───%lc", divlookup[3 * pos + 1]);
  }
  wprintf(L"───%lc\n", divlookup[3 * pos + 2]);
}

void wprinttbl(wtbl *wtbl) {
  const wchar_t empty = '\0';
  if (wtbl->print_row_top) {
    wprintonlyif(L"  ", wtbl->print_col_left);
    wprintonlyif(L" ", !wtbl->print_col_left);
    for (int i = 0; wtbl->print_row_top && i < wtbl->colc; i++) {
      printansii(wtbl->index_row_top[i]->col);
      wprintf(L"  %lc ", wtbl->index_row_top[i]->ch);
      printansii(_ansii_esc_RESET);
    }
    wprintf(L"\n");
  }
  wprintdiv(wtbl, wtbl_divpos_TOP, wtbl->colc);
  for (int i = 0; i < wtbl->rowc; i++) {
    printansii(wtbl->index_col_left[i]->col);
    wprintf(L"%lc", wtbl->print_col_left ? wtbl->index_col_left[i]->ch : empty);
    printansii(_ansii_esc_RESET);
    wprintonlyif(L" │ ", wtbl->print_col_left);
    for (int j = 0; j < wtbl->colc; j++) {
      int index = wtbl->rowc * i + j;
      printansii(wtbl->el[index]->col);
      wprintf(L"%lc", wtbl->el[index]->ch);
      printansii(_ansii_esc_RESET);
      wprintf(L" │ ");
    }
    printansii(wtbl->index_col_right[i]->col);
    wprintf(L"%lc\n",
            wtbl->print_col_right ? wtbl->index_col_right[i]->ch : empty);
    printansii(_ansii_esc_RESET);
    if (i != wtbl->rowc - 1) {
      wprintdiv(wtbl, wtbl_divpos_MIDDLE, wtbl->colc);
    }
  };
  wprintdiv(wtbl, wtbl_divpos_BOTTOM, wtbl->colc);
  if (wtbl->print_row_bottom) {
    wprintonlyif(L"  ", wtbl->print_col_left);
    wprintonlyif(L" ", !wtbl->print_col_left);
    for (int i = 0; wtbl->print_row_bottom && i < wtbl->colc; i++) {
      printansii(wtbl->index_row_bottom[i]->col);
      wprintf(L"  %c ", wtbl->index_row_bottom[i]->ch);
      printansii(_ansii_esc_RESET);
    }
    wprintf(L"\n");
  }
}
