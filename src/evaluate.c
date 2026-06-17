#include "evaluate.h"

double strtodouble(const char *str, int beg, int end) {
    double res;
    int len = end - beg;
    char sub_str[len + 1];
    for (int i = 0; i < len; i++) {
        sub_str[i] = str[i + beg];
    }
    sub_str[len] = '\0';
    sscanf(sub_str, "%lf", &res);
    return res;
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
    }
}

Token *combine(char op, Token *left, Token *right) {
    Token *result;
    if (left->type == ERROR) {
        free(right);
        return left;
    }
    if (right->type == ERROR) {
        free(left);
        return right;
    }
    result = new_token(VALUE, '\0', 0, 0);
    switch (op) {
    case '+' :
        result->val = left->val + right->val;
        break;
    case '-' :
        result->val = left->val - right->val;
        break;
    case '*' :
        result->val = left->val * right->val;
        break;
    case '/' :
        if (right->val == 0) {
            result->type = ERROR;
            result->err = DIVIDE_BY_ZERO;
        } else {
            result->val = left->val / right->val;
        }
        break;
    }
    free(left);
    free(right);
    return result;
}

int eval_str(const char *str, char *res, int str_len, int res_size) {
    Token *chain = tokenize(str, str_len);
    Exp *exp = parse(&chain, 0);
    Token *result = eval(exp);
    if (result->type == ERROR) {
        return snprintf(res, res_size, STR_ERR(result->err));
    }
    return snprintf(res, res_size, "%lf", result->val); // TODO gerer les erreurs
}

Token *tokenize(const char *str, int str_len) {
    Token *chain = new_token(OPERATOR, '(', 0, 0);
    Token *curr = chain;

    for (int i = 0, n_beg = 0; i < str_len; i++, n_beg++) {
        switch (str[i]) {
        case '+' :
        case '-' :
        case '*' :
        case '/' :
            curr->next = new_token(VALUE, '\0', strtodouble(str, n_beg, i), 0);
            n_beg = i;
            curr = curr->next;
        case '(' :
        case ')' :
            curr->next = new_token(OPERATOR, str[i], 0, 0);
            curr = curr->next;
            break;
        case ' ' :
        case '\0' :
            curr->next = new_token(VALUE, '\0', strtodouble(str, n_beg, i), 0);
            n_beg = i;
            curr = curr->next;
            break;
        default :
            if ((str[i] >= '0' && str[i] <= '9') || str[i] == '.') {
                n_beg--;
            } else {
                free_chain(chain);
                return new_token(ERROR, '\0', 0, SYNTHAX);
            }
            break;
        }
    }

    curr->next = new_token(OPERATOR, ')', 0, 0);
    return chain;
}

Exp *parse(Token **chain, float min_bp) {
    Token *t_left = *chain;
    if (*chain == NULL) {
        return new_exp(ATOM,
                       new_token(ERROR, '\0', 0, SYNTHAX), 
                       '\0', NULL, NULL);
    }
    *chain = (*chain)->next;
    Exp *left, *right;
    Token *t_op;
    char op;
    float l_bp, r_bp;
    if (t_left->type == OPERATOR) {
        if (t_left->op == '(') {
            free(t_left);
            left = parse(chain, 0.0);
            // if (!(*chain) || (*chain)->type != OPERATOR || (*chain)->op != ')') {
            //     ;// TODO raise parenthese error
            // }
            if ((*chain)) {
                t_left = *chain;
                *chain = (*chain)->next;
                free(t_left);
            }
        } else {
            left = new_exp(ATOM, 
                           new_token(ERROR, '\0', 0, BAD_TOKEN), 
                           '\0', NULL, NULL);
            free(t_left);
        }
    } else {
        left = new_exp(ATOM, t_left, '\0', NULL, NULL);
    }
    while (1) {
        t_op = *chain;
        if (t_op == NULL) {
            break;
        } else if (t_op->type == ERROR) {
            ;// TODO propager erreur
        } else if (t_op->type != OPERATOR) {
            ;// TODO raise error
        } else if (t_op->op == ')') {
            break;
        }
        op = t_op->op;
        *chain = (*chain)->next;
        free(t_op);
        get_binding_power(op, &l_bp, &r_bp);
        if (l_bp < min_bp) {
            break;
        }
        right = parse(chain, r_bp);
        left = new_exp(OPERATION, NULL, op, left, right);
    }
    return left;
}

Token *eval(Exp *exp) {
    Token *result = NULL;
    switch (exp->type) {
    case OPERATION :
        result = combine(exp->op, eval(exp->left), eval(exp->right));
        free(exp->left);
        free(exp->right);
        break;
    case ATOM :
        result = exp->val;
        break;
    }
    return result;
}

void free_chain(Token *chain) {
    Token *del = NULL;
    if (chain == NULL) {
        return;
    }

    while (chain->next != NULL) {
        del = chain->next;
        chain->next = del->next;
        free(del);
    }
    free(chain);
}

Token *new_token(token_t type, char op, double val, error_t err) {
    Token *t = malloc(sizeof(Token));
    if (t) {
        t->type = type;
        switch (type) {
        case OPERATOR :
            t->op = op;
            break;
        case VALUE :
            t->val = val;
            break;
        case ERROR :
            t->err = err;
            break;
        }
        t->next = NULL;
    }
    return t;
}

Exp *new_exp(exp_t type, Token *val, char op, Exp *left, Exp *right) {
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
