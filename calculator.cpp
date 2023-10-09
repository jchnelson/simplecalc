#include "calculator.h"

#include <map>
#include <vector>
#include <string>
#include <functional>
#include <algorithm>
#include <cmath>
#include <stdexcept>

#include <QDebug>

using std::map;
using std::vector;
using std::function;
using std::string;

std::map<char, function<double(double,double)>> Calculator::make_arith()
{
    
    std::map<char, function<double(double,double)>> arith;
    
    function<double(double,double)> add = [] (double a, double b) { return a + b; };
    function<double(double,double)> sub = [] (double a, double b) { return a - b; };
    function<double(double,double)> mul = [] (double a, double b) { return a * b; };
    function<double(double,double)> div = [] (double a, double b) { return a / b; };
    function<double(double,double)> exp = [] (double a, double b) { return pow(a,b); };
    
    arith.insert({'+', add});
    arith.insert({'-', sub});
    arith.insert({'*', mul});
    arith.insert({'/', div});
    arith.insert({'e', exp});
    
    return arith;
}

std::pair<string,string> parse_expr_left(const string& s)
{ // leave last operand on lhs
    size_t index = s.find_last_of("e*/+-");
    string lhs(s, 0, index+1);
    string rhs(s, index+1);
    return {lhs, rhs};
    
}

std::pair<string,string> parse_expr_right(const string& s)
{  // leave first operand on rhs
    size_t index = s.find_first_of("e*/+-");
    string lhs(s, 0, index);
    string rhs(s, index);
    return {lhs, rhs};
    
}

bool check_expr(const string& s)
{
    return all_of(s.cbegin(), s.cend(), [] (const char c)
                  { return isdigit(c) || isspace(c) || c == '.'; });
}

string single_calc(const string& s, const char c,
                   function<double(double,double)> func)
{
    // negative numbers need to be handled.  Unary '-'
    // should be identified by there being no number to the left of
    // it, it should either be a parenthesis, another operator,
    // or nothing. Unary '+' should be handled the same way if present
    
    // I might have to implement parentheses to implement negative
    // numbers properly.  To parse an expression correctly, it would
    // seem easiest to insert parentheses around the negative number
    
    qInfo() << "string passed to single_calc was" << s;
    if (s.find(c) != string::npos)
    {
        qInfo() << c;
        size_t index = s.find(c);
        string lhs(s, 0, index);
        string rhs(s, index+1);
        string l_tojoin;
        string r_tojoin;
        string l_operand;
        string r_operand;
        if (!check_expr(lhs))
        {
            qInfo() << "additional operator on left" << lhs;
            auto lpair = parse_expr_left(lhs);
            l_tojoin = lpair.first;
            l_operand = lpair.second;
        } 
        else
            l_operand = lhs;
        if (!check_expr(rhs))
        {
            qInfo() << "additional operator on right" << rhs;
            auto rpair = parse_expr_right(rhs);
            r_tojoin = rpair.second;
            r_operand = rpair.first;
        }
        else
            r_operand = rhs;
        
        double l_oper = std::stod(l_operand);
        double r_oper = std::stod(r_operand);
        qInfo() << l_oper << c << r_oper;
        double result = func(l_oper, r_oper);
        string res_s = std::to_string(result);
        qInfo() << res_s;
        qInfo() << l_tojoin + res_s + r_tojoin;
        
        return l_tojoin + res_s + r_tojoin; // if they're empty that's fine
        
    }
    return s;
}

string Calculator::do_order_op(const string& s)
{
    string expr = s;
    while (expr.find('e') != string::npos)
    {
        expr = single_calc(expr, 'e', arith['e']);
    }
    while (expr.find('*') != string::npos ||
           (expr.find('/') != string::npos))
    {
        if (expr.find('*') < expr.find('/'))
        {
            expr = single_calc(expr, '*', arith['*']);
        }
        else
            expr = single_calc(expr, '/', arith['/']);
    }
    while (expr.find('+') != string::npos ||
           (expr.find('-') != string::npos))
    {
        if (expr.find('+') < expr.find('-'))
        {
            expr = single_calc(expr, '+', arith['+']);
        }
        else
            expr = single_calc(expr, '-', arith['-']);
    }
    return expr;
}

string Calculator::parse_parenth(const string& s)
{
    // this only runs if parentheses are found.  It is not yet known
    // how many, or whether the expression is valid. Check for a valid expression
    // first, then find subexpressions inside parentheses, which themselves
    // contain no parentheses, then pass those to calculate?? I might have
    // to rethink my flow afterward
    qInfo() << "inside parse_parenth";
    string expr = s;
    string subexpr;
    string expr_left;
    string expr_right;
    vector<size_t> openers;
    
    for (size_t i = 0; i != expr.size(); ++i)
    {
        
        if (expr[i] == '(')
        {
            openers.push_back(i);
        }
        if (expr[i] == ')')
        {
            if (openers.empty())
                throw std::runtime_error("Invalid Expression");
            else
            {
                size_t subexpr_size = (i + 1) - openers.back() ;
                subexpr = string(expr, openers.back(), subexpr_size);
                expr_left = string(expr, 0, openers.back());
                if (openers.back() + subexpr_size < expr.size())
                    expr_right = string(expr, (openers.back() + subexpr_size));
                qInfo() << "subexpression is" << subexpr;
                subexpr = subexpr.substr(1, subexpr.size() -2);
                qInfo() << expr_left << subexpr << expr_right;
                if (subexpr.find_first_of("()") == string::npos)
                {
                    qInfo() << "no other parentheses";
                    subexpr = do_order_op(subexpr);
                    qInfo() << "processed subexpr as " << subexpr;
                }
                openers.pop_back();
                break;
            }
        }
    }
    return expr_left + subexpr + expr_right;
}

QString Calculator::calculate(const string& s)
{
    
    string expr = s;
    qInfo() << "top of calculate";
    int count = 0;
    while (expr.find_first_of("()") != string::npos)
    {
        ++count;
        qInfo() << "parentheses found" << expr;
        expr = parse_parenth(expr);
        if (count == 10) break;
        
    }
    
    expr = do_order_op(expr);
    
    // do e, then */ together,  then +- together

    
    qInfo() << expr;
    qInfo() << QString::number(std::stod(expr), 'f', 3);
    return QString::number(std::stod(expr), 'f', 3);
}
