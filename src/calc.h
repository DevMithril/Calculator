#ifndef _CALC_H
#define _CALC_H
#include <stdlib.h>
#include <ncurses.h>
#include "evaluate.h"

#define CTRL_KEY(key) ((key) & 0x1f)
#define MAX_LEN 48

typedef struct Calc {
    bool quit;
    char exp[MAX_LEN];
    int exp_len;
    char res[MAX_LEN];
    int res_len;
} Calc;

void init_calc(Calc *calc);

void run_calc(Calc *calc);

void quit_calc(Calc *calc, int status);

#endif
