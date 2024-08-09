#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include "pprint.h"
#include "chutils.h"
#include "mathutils.h"
#include "rules.h"
#include "render.h"

void prompt(chlogtl *logtl) {
    char player[10], state[5];
    strcpy(player, (logtl->player==PURPLE) ? "purple" : "green");
    strcpy(state, (logtl->player_ini_state==false) ? "from" : "to");
    printf("%s:%s> ", player, state);
}

void switchplayer(chlogtl *logtl) {
    logtl->player=(logtl->player==PURPLE)?GREEN:PURPLE;
}

void render_piece(chlogtl *logtl, chpiece piece) {
    wchar_t table[13]={L' ', L'♙', L'♔', L'♕', L'♗', L'♘', L'♖',
                        L'♙', L'♔', L'♕', L'♗', L'♘', L'♖'};
    if (piece==NIL) printf("%c", table[piece]);
    else if (piece<=6) {
        purple();
        wprintf(L"%lc", table[piece]);
    }
    else {
        green();
        wprintf(L"%lc", table[piece]);
    }
   reset();
}

void render_log(chlogtl *logtl) {
    if (logtl->msgocc) printf("MSG: %s\n", logtl->msg);
    if (logtl->errocc) printf("ERR: %s\n", logtl->errmsg);
    if (logtl->msgocc || logtl->errocc) printf("---\n\n");
    logtl->msgocc=false;
    logtl->errocc=false;
    strcpy(logtl->msg, "\0");
    strcpy(logtl->errmsg, "\0");
}

void render_board(chlogtl *logtl, chpiece** board) {
    printf("     ");
    for (int i=0; i<8; i++) {
        printf("%c   ", (char)(i+'a'));
    }
    printf("\n");
    
    printf("   ");
    for (int i=0; i<8; i++) {
        printf("+---");
    }
    printf("+\n");
    
    for (int i=7; i>=0; i--) {
        printf(" %d | ", i+1);
        for (int j=0; j<8; j++) {
            render_piece(logtl, board[i][j]);
            printf(" | ");
        }
        printf("%d\n   ", i+1);
        for (int j=0; j<8; j++) {
            printf("+---");
        }
        printf("+\n");
    }
    
    printf("     ");
    for (int i=0; i<8; i++) {
        printf("%c   ", (char)(i+'a'));
    }
    printf("\n");
}

void showpt(point* p) {
    printf("point: x=%d, y=%d\n", p->x, p->y);
}

bool readmov(chlogtl *logtl, char *inp, point *p) {
    int size=strlen(inp);
    if (size!=3) return false;
    if (inp[0]<'a' || inp[0]>'h' || inp[1]<'1' || inp[1]>'8') return false;
    p->y=inp[0]-'a';
    p->x=inp[1]-'1';
    return true;
}

bool parsecmd(chlogtl *logtl, chpiece** board, char *inp) {
    int size=strlen(inp);
    if (size<1) return false;

    char ch=inp[0];
    if (ch=='q') logtl->exit=true;
    else if (ch=='?') {
        msg(logtl, "Here's a list of commands:\n\nq - quit\n? - help\n> - possible moves (eg, >b2)\n");
    }
    else return false;
    return true;
}

char* readinput() {
    const int size=200;
    char *buf=(char *)malloc(size*sizeof(char));
    fgets(buf, size, stdin);
   
    char *input=(char *)malloc(strlen(buf)*sizeof(char));
    strcpy(input, buf);
    free(buf);
    return input;
}

void render(chlogtl *logtl, chpiece** board) {
    clear();
    
    if (logtl->exit==true) return;
    if (logtl->player_ini_state==false && logtl->player_final_state==true) {
        logtl->player_ini_state=false;
        logtl->player_final_state=false;
    }
    
    render_log(logtl);
    render_board(logtl, board);
    printf("\n");

    point *p;
    p=(logtl->player_ini_state==false) ? logtl->from : logtl->to;

    prompt(logtl);
    char *line=readinput();
    bool cmdstatus=parsecmd(logtl, board, line);
    if (cmdstatus==true) {
        render(logtl, board);
        return;
    }

    bool readmovstatus=readmov(logtl, line, p);
    if (logtl->player_ini_state==true) logtl->player_final_state=readmovstatus;
    else logtl->player_ini_state=readmovstatus;

    if (logtl->player_ini_state==false || logtl->player_final_state==false) {
        render(logtl, board);
        return;
    }

    bool valid=checkmov(logtl, board, logtl->from, logtl->to);
    if (valid) {
        pmovpiece(board, logtl->from, logtl->to);
        logtl->movc++;
        switchplayer(logtl);
        logtl->player_ini_state=false;
        logtl->player_final_state=false;
    }

    render(logtl, board);
}

void init(chlogtl *logtl, chpiece** board) {
    // pawns
    int i=1;
    for (int j=0; j<8; j++) {
        addpiece(board, RPAWN, i, j);
    }
    addpiece(board, RKING, logtl->rkx, logtl->rky);
    addpiece(board, RQUEEN, 0, 4);
    addpiece(board, RBISHOP, 0, 2);
    addpiece(board, RBISHOP, 0, 5);
    addpiece(board, RKNIGHT, 0, 1);
    addpiece(board, RKNIGHT, 0, 6);
    addpiece(board, RROOK, 0, 0);
    addpiece(board, RROOK, 0, 7);
    
    // pawns
    i=6;
    for (int j=0; j<8; j++) {
        addpiece(board, GPAWN, i, j);
    }
    addpiece(board, GKING, logtl->gkx, logtl->gky);
    addpiece(board, GQUEEN, 7, 3);
    addpiece(board, GBISHOP, 7, 2);
    addpiece(board, GBISHOP, 7, 5);
    addpiece(board, GKNIGHT, 7, 1);
    addpiece(board, GKNIGHT, 7, 6);
    addpiece(board, GROOK, 7, 0);
    addpiece(board, GROOK, 7, 7);
}
