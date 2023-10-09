#include "calculator.h"

#include <map>
#include <string>
#include <functional>
#include <algorithm>
#include <cmath>

#include <QDebug>

using std::map;
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
    qInfo() << "string parsed as" << lhs << rhs;
    return {lhs, rhs};
    
}

std::pair<string,string> parse_expr_right(const string& s)
{  // leave first operand on rhs
    size_t index = s.find_first_of("e*/+-");
    string lhs(s, 0, index);
    string rhs(s, index);
    qInfo() << "string parsed as" << lhs << rhs;
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
            qInfo() << l_tojoin;
            qInfo() << l_operand;
        } 
        else
            l_operand = lhs;
        if (!check_expr(rhs))
        {
            qInfo() << "additional operator on right" << rhs;
            auto rpair = parse_expr_right(rhs);
            r_tojoin = rpair.second;
            r_operand = rpair.first;
            qInfo() << r_tojoin;
            qInfo() << r_operand;
        }
        else
            r_operand = rhs;
        
        double l_oper = std::stod(l_operand);
        double r_oper = std::stod(r_operand);
        qInfo() << l_oper << c << r_oper;
        double result = func(l_oper, r_oper);
        qInfo() << result;
        string res_s = std::to_string(result);
        qInfo() << res_s;
        qInfo() << l_tojoin + res_s + r_tojoin;
        
        return l_tojoin + ' ' + res_s + ' ' + r_tojoin; // if they're empty that's fine
        
    }
    return s;
}

double Calculator::calculate(const string& s)
{
    
    string expr = s;
    string::size_type index1 = 0;
    string::size_type index2 = 0;
    
    // do e, then */ together,  then +- together

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
    return std::stod(expr);
}
