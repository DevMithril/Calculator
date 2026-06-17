#include "evaluate.h"

int strtodouble(const char *str, int beg, int end, double *res) {
    int len = end - beg;
    char sub_str[len + 1];

    for (int i = 0; i < len; i++) {
        sub_str[i] = str[i + beg];
    }

    if (res && len > 0) {
        sub_str[len] = '\0';
        sscanf(sub_str, "%lf", res);
        return 1;
    }
    return 0;
}

void get_binding_power(char op, float *l_bp, float *r_bp) {
    switch (op) {
    case '+' :
    case '-' :
        *l_bp = 1;
        *r_bp = 1.1;
        break;
    case '*' :
    case '/' :
        *l_bp = 2;
        *r_bp = 2.1;
        break;
    default :
        *l_bp = *r_bp = 0;
    }
}

Token combine(char op, Token left, Token right) {
    Token res;
    if (left.type == ERROR) {
        return left;
    }
    if (right.type == ERROR) {
        return right;
    }
    switch (op) {
    case '+' :
        res = token(VALUE, left.val + right.val);
        break;
    case '-' :
        res = token(VALUE, left.val - right.val);
        break;
    case '*' :
        res = token(VALUE, left.val * right.val);
        break;
    case '/' :
        if (right.val == 0) {
            res = token(ERROR, DIVIDE_BY_ZERO);
        } else {
            res = token (VALUE, left.val / right.val);
        }
        break;
    }
    return res;
}

int eval_str(const char *str, char *res, int str_len, int res_size) {
    Lexer *lexer = tokenize(str, str_len);
    Exp *exp = parse(lexer, 0);
    if (lexer->cur != lexer->size) {
        free(lexer);
        return snprintf(res, res_size, STR_ERR(PARENTHESE));
    }
    free(lexer);
    Token result = eval(exp);
    if (result.type != ERROR) {
        return snprintf(res, res_size, "%lf", result.val);
    }
    return snprintf(res, res_size, STR_ERR(result.err));
}

Lexer *tokenize(const char *str, int str_len) {
    Token tokens[str_len];
    int cur = 0;
    double number;

    for (int i = 0, beg = 0; i < str_len; i++, beg++) {
        if ((str[i] >= '0' && str[i] <= '9') || str[i] == '.') {
            beg--;
        } else {
            if (strtodouble(str, beg, i, &number)) {
                tokens[cur++] = token(VALUE, number);
            }
            if (str[i] != ' ' && str[i] != '\0'){
                tokens[cur++] = token(OPERATOR, (double)(str[i]));
            }
            beg = i;
        }
    }

    Lexer *lexer = malloc(sizeof(Lexer) + sizeof(Token) * cur);
    lexer->size = cur;
    lexer->cur = 0;
    for (int i = 0; i < cur; i++) {
        lexer->tokens[i] = tokens[i];
    }
    return lexer;
}

Exp *parse(Lexer *lexer, float min_bp) {
    Exp *left = NULL, *right = NULL;
    char op;
    float l_bp, r_bp;

    if (cur_token(lexer).type != OPERATOR) {
        left = new_exp(ATOM, cur_token(lexer), 0, 0, 0);
        lexer->cur++;
    } else if (cur_token(lexer).op == '(') {
        lexer->cur++;
        left = parse(lexer, 0);
        if (cur_token(lexer).type == OPERATOR && cur_token(lexer).op == ')') {
            lexer->cur++;
        } else {
            free_exp(left);
            return new_exp(ATOM, token(ERROR, (double)(PARENTHESE)), 0, 0, 0);
        }
    } else {
        return new_exp(ATOM, token(ERROR, (double)(BAD_TOKEN)), 0, 0, 0);
    }

    while (1) {
        if (lexer->cur == lexer->size ||
            (cur_token(lexer).type == OPERATOR && cur_token(lexer).op == ')')) {
            return left;
        } else if (cur_token(lexer).type == ERROR) {
            free_exp(left);
            free_exp(right);
            return new_exp(ATOM, cur_token(lexer), 0, 0, 0);
        } else if (cur_token(lexer).type != OPERATOR) {
            free_exp(left);
            free_exp(right);
            return new_exp(ATOM, token(ERROR, (double)(BAD_TOKEN)), 0, 0, 0);
        } else {
            op = cur_token(lexer).op;
            lexer->cur++;
        }

        get_binding_power(op, &l_bp, &r_bp);
        if (l_bp < min_bp) {
            return left;
        }
        right = parse(lexer, r_bp);
        left = new_exp(OPERATION, token(0,0), op, left, right);
    }
}

Token eval(Exp *exp) {
    Token res;
    switch (exp->type) {
    case OPERATION :
        res = combine(exp->op, eval(exp->left), eval(exp->right));
        break;
    case ATOM :
        res = exp->val;
        free(exp);
        break;
    }
    return res;
}

Token token(token_t type, double data) {
    Token t;
    t.type = type;
    switch (type) {
    case OPERATOR :
        t.op = (char)data;
        break;
    case VALUE :
        t.val = data;
        break;
    case ERROR :
        t.err = (error_t)data;
        break;
    }
    return t;
}

Exp *new_exp(exp_t type, Token val, char op, Exp *left, Exp *right) {
    Exp *e = malloc(sizeof(Exp));
    if (e) {
        e->type = type;
        switch (type) {
        case ATOM :
            e->val = val;
            break;
        case OPERATION :
            e->op = op;
            e->left = left;
            e->right = right;
            break;
        }
    }
    return e;
}

void free_exp(Exp *exp) {
    if (exp) {
        if (exp->type == OPERATION) {
            free_exp(exp->left);
            free_exp(exp->right);
        }
        free(exp);
    }
}
