#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <map>
#include <string>
#include <functional>
#include <QString>

class Calculator
{
public:
    Calculator() = default;
    QString calculate(const std::string&);
    
private:
    double currval = 0;
    std::map<char, std::function<double(double,double)>> make_arith();
    std::map<char, std::function<double(double,double)>> arith = make_arith();
    std::string do_order_op(const std::string& s);
    std::string parse_parenth(const std::string& s);
};

#endif // CALCULATOR_H
