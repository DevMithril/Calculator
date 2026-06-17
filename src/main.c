#include "calc.h"

int main(void) {
    Calc calc;
    init_calc(&calc);

    while (!calc.quit) {
        run_calc(&calc);
    }

    quit_calc(&calc, 0);
}
