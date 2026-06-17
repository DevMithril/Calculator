#ifndef _EVALUATE_H
#define _EVALUATE_H
#include <stdio.h>
#include <stdlib.h>

typedef enum token_t {
    OPERATOR,
    VALUE,
    ERROR
} token_t;

typedef enum error_t {
    SYNTHAX,
    MISSMATCH_PARENTHESE,
    DIVIDE_BY_ZERO,
    BAD_TOKEN
} error_t;

typedef enum exp_t {
    ATOM,
    OPERATION
} exp_t;

#define STR_ERR(err) (char *) {                                             \
    ((error_t)(err) == SYNTHAX) ? "Err: Bad synthax" :                      \
    ((error_t)(err) == MISSMATCH_PARENTHESE) ? "Err: Missmatch parenthese" :\
    ((error_t)(err) == DIVIDE_BY_ZERO) ? "Err: Dividing by zero" :          \
    ((error_t)(err) == BAD_TOKEN) ? "Err: Bad token" : "Err: Unknown"}

typedef struct Token {
    token_t type;
    union {
        char op;
        double val;
        error_t err;
    };
    struct Token *next;
} Token;

typedef struct Exp {
    exp_t type;
    union {
        Token *val;
        struct {
            char op;
            struct Exp *left;
            struct Exp *right;
        };
    };
} Exp;

double strtodouble(const char *str, int beg, int end);

void get_binding_power(char op, float *l_bp, float *r_bp);

int eval_str(const char *str, char *res, int str_len, int res_size);

Token *tokenize(const char *str, int str_len);

Exp *parse(Token **chain, float min_bp);

Token *eval(Exp *exp);

void free_chain(Token *chain);

Token *new_token(token_t type, char op, double val, error_t err);

Exp *new_exp(exp_t type, Token *val, char op, Exp *left, Exp *right);

#endif
