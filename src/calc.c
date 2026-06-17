#include "calc.h"

void draw_calc() {
    // Lignes, colonnes
    int i;
    for (i = 0; i <= 50; i+=10) {
        mvvline(10, i, ACS_VLINE, 7);
    }
    mvvline(1, 0, ACS_VLINE, 8);
    mvvline(1, 50, ACS_VLINE, 8);
    for (i = 9; i <= 17; i+=2) {
        mvaddch(i, 0, ACS_LTEE);
        mvhline(i, 1, ACS_HLINE, 49);
        mvaddch(i, 50, ACS_RTEE);
    }
    mvhline(0, 1, ACS_HLINE, 49);
    for (i = 10; i < 50; i+=10) {
        mvaddch(9, i, ACS_TTEE);
        mvaddch(11, i, ACS_PLUS);
        mvaddch(13, i, ACS_PLUS);
        mvaddch(15, i, ACS_PLUS);
        mvaddch(17, i, ACS_BTEE);
    }
    mvaddch(0, 0, ACS_ULCORNER);
    mvaddch(0, 50, ACS_URCORNER);
    mvaddch(17, 0, ACS_LLCORNER);
    mvaddch(17, 50, ACS_LRCORNER);

    // Texte
    mvaddch(10, 5, '7');
    mvaddch(10, 15, '8');
    mvaddch(10, 25, '9');
    mvaddch(10, 35, '(');
    mvaddch(10, 45, ')');
    mvaddch(12, 5, '4');
    mvaddch(12, 15, '5');
    mvaddch(12, 25, '6');
    mvaddch(12, 35, '*');
    mvaddch(12, 45, '/');
    mvaddch(14, 5, '1');
    mvaddch(14, 15, '2');
    mvaddch(14, 25, '3');
    mvaddch(14, 35, '+');
    mvaddch(14, 45, '-');
    move(16, 4);
    printw("DEL");
    mvaddch(16, 15, '0');
    move(16, 23);
    printw("ENTER");
    move(1, 1);
    refresh();
}

void init_calc(Calc *calc) {
    initscr();
    cbreak();
    noecho();
    noqiflush();
    keypad(stdscr, TRUE);
    mousemask(BUTTON1_CLICKED, NULL);
    draw_calc();
    calc->quit = false;
    calc->exp[0] = '\0';
    calc->exp_len = 0;
    calc->res[0] = '\0';
    calc->res_len = 0;
}

void refresh_screen(Calc *calc) {
    for (int x = 1; x < 50; x++) {
        for (int y = 1; y < 9; y++) {
            mvaddch(y, x, ' ');
        }
    }
    move(2, 1);
    printw(calc->res);
    move(1, 1);
    printw(calc->exp);
    refresh();
}

bool is_valid_char(int key) {
    return (key >= '0' && key <= '9')
        || (key == '+')
        || (key == '-')
        || (key == '*')
        || (key == '/')
        || (key == '(')
        || (key == ')');
}

void convert_mouse_into_key() {
    int keys[4][5] = {
        {'7', '8', '9', '(', ')'},
        {'4', '5', '6', '*', '/'},
        {'1', '2', '3', '+', '-'},
        {KEY_BACKSPACE, '0', '\n', '\0', '\0'}
    };
    MEVENT m;
    int x, y;
    getmouse(&m);
    x = (m.x - 1)/10;
    y = (m.y - 10)/2;
    ungetch(keys[y][x]);
}

void addtoexp(Calc *calc, int key) {
    calc->exp[calc->exp_len] = (char)(key);
    if (calc->exp_len < MAX_LEN - 2) {
        calc->exp_len++;
    }
    calc->exp[calc->exp_len] = '\0';
}

void delfromexp(Calc *calc) {
    if (calc->exp_len > 0) {
        calc->exp_len--;
    }
    calc->exp[calc->exp_len] = '\0';
}

void run_calc(Calc *calc) {
    int key = getch();
    if (key == CTRL_KEY('w')) {
        calc->quit = true;
    } else if (key == KEY_MOUSE){
        convert_mouse_into_key();
    } else if (is_valid_char(key)) {
        addtoexp(calc, key);
        refresh_screen(calc);
    } else if (key == KEY_BACKSPACE) {
        delfromexp(calc);
        refresh_screen(calc);
    } else if (key == '\n') {
        calc->res_len = eval_str(calc->exp, calc->res, calc->exp_len + 1, MAX_LEN);
        refresh_screen(calc);
    }
}

void quit_calc(Calc *calc, int status) {
    (void)calc;
    endwin();
    exit(status);
}
