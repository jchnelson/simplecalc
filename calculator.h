#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <map>
#include <string>
#include <functional>

class Calculator
{
public:
    Calculator() = default;
    double calculate(const std::string&);
    
private:
    double currval = 0;
    std::map<char, std::function<double(double,double)>> make_arith();
    std::map<char, std::function<double(double,double)>> arith = make_arith();
};

#endif // CALCULATOR_H
