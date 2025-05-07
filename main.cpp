#include <iostream>
#include <string>
#include <stack>

using namespace std;

/*
Supprime les espaces d'une chaîne de caractères
\param str la chaîne de caractères
\return une nouvelle chaîne de caractère sans espaces
*/
string remove_spaces(string str)
{
    string new_str;
    for (auto car : str)
    {
        if (car != ' ')
        {
            new_str.push_back(car);
        }
    }
    return new_str;
}

/*
Extrait un opérateur et son second opérande d'une expression mathématique
\param expr référence de la chaîne de caractère contenant l'expression mathématique
\return une chaîne de caractère contenant l'opérateur et l'opérande
\note l'expression original est amputée du retour de cette fonction
*/
string get_op(string &expr)
{
    string op;
    int i = expr.size()-1;
    
    while (0 < i && expr.at(i) != '+' && expr.at(i) != '-' && expr.at(i) != '*' && expr.at(i) != '/')
    {
        i--;
    }

    op = expr.substr(i, expr.size());
    expr = expr.substr(0, i);
    
    return op;
}

/*
Converti un opérande en double
\param op chaîne de caractère contenant un duo opérateur/opérande
\return la valeur de l'opérande
\note l'opérateur est ici ignoré, le retour de cette fonction est donc toujours positif ou null
*/
double convert_op_to_d(string op)
{
    double num = 0;
    int len_op = op.size();
    bool digit = true;
    int number_of_digits_after_dot = 0;

    for (int i = 1; i < len_op; i++)
    {
        if (op.at(i) == '.' || op.at(i) == ',')
        {
            digit = false;
            continue;
        }
        if (digit)
        {
            num = 10 * num + (double)(op.at(i) - '0');
        }
        else
        {
            number_of_digits_after_dot++;
            num = num + (double)(op.at(i) - '0') / (10 * number_of_digits_after_dot);
        }
    }

    return num;
}

/*
Extrait une valeur numérique d'une expression mathématique simple
\param expression chaîne de caractères contenant l'expression mathématique
\return un élément de la somme permettant d'obtenir le résultat du calcul de l'expression
*/
double get_number(string &expression)
{
    string op = get_op(expression);
    double num = convert_op_to_d(op);

    switch (op.at(0))
    {
    case '+':
        return num;
    case '-':
        return -num;
    case '*':
        return get_number(expression) * num;
    case '/':
        return (num == 0) ? 0 : get_number(expression) / num;
    default:
        return 0;
    }
}

/* 
Donne le résultat d'un calcul mathématique simple
\param expression chaîne de caractères correspondant à l'expression à calculer
\return le résultat du calcul
\note le paramètre est vidé durant l'exécution de cette fonction
*/
double calc(string expression)
{
    double res = 0;
    stack<double> stck;
    expression = remove_spaces("+" + expression);

    while (!expression.empty())
    {
        stck.push(get_number(expression));
    }
    
    while (!stck.empty())
    {
        res += stck.top();
        stck.pop();
    }
    
    return res;
}

int main(void)
{
    string expr;
    bool quit = false;
    
    while (!quit)
    {
        getline(cin, expr);
        if (expr == "exit" || expr == "quit")
        {
            quit = true;
        }
        else
        {
            cout << calc(expr) << endl;
        }
    }

    return EXIT_SUCCESS;
}