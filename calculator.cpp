#include "calculator.h"

#include <map>
#include <string>
#include <functional>
#include <cmath>

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
    arith.insert({'^', exp});
    
    return arith;
}

double Calculator::calculate(const string& s)
{
    double ret = 0;
    
    string::size_type index;
    double lhs;
    double rhs;
    char found_function;
    for (const auto& fpair : arith)
    {
        
        if ((index = s.find(fpair.first)) != string::npos)
        {
            found_function = fpair.first;
            break;
        }
    } 
    lhs = std::stod(string(s, 0, index));
    rhs = std::stod(string(s, index+1));
    
    currval = ret = arith[found_function](lhs, rhs);
    
    return ret;
    
}