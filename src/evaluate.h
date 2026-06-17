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
    PARENTHESE,
    DIVIDE_BY_ZERO,
    BAD_TOKEN
} error_t;

typedef enum exp_t {
    ATOM,
    OPERATION
} exp_t;

#define STR_ERR(err) (char *) {                                         \
    ((error_t)(err) == SYNTHAX) ? "Err: Synthax" :                      \
    ((error_t)(err) == PARENTHESE) ? "Err: Missmatched parenthese" :    \
    ((error_t)(err) == DIVIDE_BY_ZERO) ? "Err: Dividing by zero" :      \
    ((error_t)(err) == BAD_TOKEN) ? "Err: Bad token" : "Err: Unknown"}

typedef struct Token {
    token_t type;
    union {
        char op;
        double val;
        error_t err;
    };
} Token;

typedef struct Lexer {
    int size;
    int cur;
    Token tokens[];
} Lexer;

typedef struct Exp {
    exp_t type;
    union {
        Token val;
        struct {
            char op;
            struct Exp *left;
            struct Exp *right;
        };
    };
} Exp;

#define cur_token(lex) ((Lexer*)(lex))->tokens[((Lexer*)(lex))->cur]

int strtodouble(const char *str, int beg, int end, double *res);

void get_binding_power(char op, float *l_bp, float *r_bp);

Token combine(char op, Token left, Token right);

int eval_str(const char *str, char *res, int str_len, int res_size);

Lexer *tokenize(const char *str, int str_len);

Exp *parse(Lexer *lexer, float min_bp);

Token eval(Exp *exp);

Token token(token_t type, double data);

Exp *new_exp(exp_t type, Token val, char op, Exp *left, Exp *right);

void free_exp(Exp *exp);

#endif
