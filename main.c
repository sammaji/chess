#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <wchar.h>
#include <locale.h>
#include <unistd.h>
#include "pprint.h"
#include "mathutils.h"
#include "chutils.h"
#include "rules.h"
#include "render.h"

int main() {
    setlocale(LC_CTYPE, "");

    chpiece** board=zeros();
    
    chlogtl *logtl=(chlogtl *)malloc(sizeof(chlogtl));
    logtl->exit=false;
    logtl->movc=0;
    logtl->player=PURPLE;
    logtl->player_ini_state=false;
    logtl->player_final_state=false;
    logtl->msgocc=false;
    logtl->errocc=false;
    logtl->msg=(char *)malloc(200*sizeof(char));
    logtl->errmsg=(char *)malloc(200*sizeof(char));
    strcpy(logtl->msg, "\0");
    strcpy(logtl->errmsg, "\0");

    logtl->rkx=0; logtl->rky=3;
    logtl->gkx=7; logtl->gky=4;
    logtl->from=(point *)malloc(sizeof(point));
    logtl->to=(point *)malloc(sizeof(point));

    msg(logtl, "Welcome to the game! Type ? for help.");
    init(logtl, board);
    render(logtl, board);

    return 0;
}
