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

bool check_unary_neg(const string& expr, size_t index)
{

    if (index == 0 || !isdigit(expr[index-1]))
        return true;
    else
        return false;
}

std::pair<string,string> parse_expr_left(const string& s)
{ // leave last operator on lhs
    size_t index = s.find_last_of("e*/+-");
    qInfo() << "initial left index" << index;
    if (index == 0 && s[index] == '-')
    {
        return {string(), s};  // s is just a negative number
    }
    else if (check_unary_neg(s, index)) // still a negative,
                                        // but check for other operator
    {
        qInfo() << "first index was negative and there's another operator";
        size_t neg_index = index;
        index = s.substr(0,neg_index).find_last_of("e*/+-");
        // this should actually be right, check from the beginning,
        // "index" number of characters.  If index is 3, it's the 4th
        // character so check the first 3 characters for the actual operator
    }
    // this index could still be a negative..
    qInfo() << "second left index" << index;
    string lhs(s, 0, index+1);
    string rhs(s, index+1);
    qInfo() << lhs << rhs;
    return {lhs, rhs};
    
}

std::pair<string,string> parse_expr_right(const string& s)
{  // leave first operator on rhs
    // if index is 0 here it doesn't mean shit
    
    // at this point the correct operator would have been selected,
    // so if there's an operator at the first index then it's a negative
    // then the next potential binary operator MUST be an actual binary operator
    size_t index = s.find_first_of("e*/+-", 1);
    if (index == string::npos)
        return {s, string()};
    string lhs(s, 0, index);
    string rhs(s, index);
    return {lhs, rhs};
    
}

bool check_expr(const string& s)
{
    // maybe add a check for negative here too???
    return all_of(s.cbegin(), s.cend(), [] (const char c)
                  { return isdigit(c) || isspace(c) || c == '.'; });
}

string single_calc(const string& s, const char c,
                   function<double(double,double)> func)
{
    
    qInfo() << "string passed to single_calc was" << s;
    // if first character is -, and char is -, look for second -
    size_t index = s[0] == '-' && c == '-' ? s.find(c,1) : s.find(c);
    if (index != string::npos)
    {
        qInfo() << c;
 
        string lhs(s, 0, index);
        string rhs(s, index+1);
        string l_tojoin;
        string r_tojoin;
        string l_operand;
        string r_operand;
        
        if (!check_expr(lhs))
        {
            qInfo() << "potential operator on left" << lhs;
            auto lpair = parse_expr_left(lhs);
            l_tojoin = lpair.first;
            l_operand = lpair.second;
        } 
        else
            l_operand = lhs;
        if (!check_expr(rhs))
        {
            qInfo() << "potential operator on right" << rhs;
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
    
    // finding the operation will execute correctly until we get to +- loop
    
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
        bool first_minus_is_neg = check_unary_neg(expr, expr.find('-'));

        size_t minus_index = first_minus_is_neg ?
                                 expr.find('-', expr.find('-')+1) : expr.find('-');
        // minus_index still potentially is the index of a unary negative
        
        if (first_minus_is_neg && minus_index == string::npos
                                   && expr.find('+') == string::npos )
            return expr;
        
        // if we're here, there's a binary operator to process

        if (expr.find('+') < minus_index)  // if this is still first, doesn't matter if
                                           // second - is unary
        {
            expr = single_calc(expr, '+', arith['+']);
        }
        else if (minus_index != expr.find('-'))
        {
            expr = single_calc(expr, '-', arith['-']);
        }
        else  // then there was no negative found, or the binary - is still first
        {
            expr = single_calc(expr, '-', arith['-']);
            // and here
        }
    }
    return expr;
}

string Calculator::parse_parenth(const string& s)
{
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
