#include "MathExpression.h"


//keep priorities like in c++ in case we will add some new operators
std::map<std::string ,int> SyntaxTree::operations_priority = {
        {"||", 15},
        {"&&", 14},
        {"^^", 12},
        {"=", 10},
        {"!=", 10},
        {">=", 9},
        {">", 9},
        {"<=", 9},
        {"<", 9},
        {"+", 6},
        {"-", 6},
        {"*", 5},
        {"/", 5},
        {"%", 5},
        {"!", 3},
        {"string", 0},
        {"int32", 0},
        {"bin", 0},
        {"true", 0},
        {"false", 0},
        {"module", 0},
        {"name", 0},
        {"complex_name", 0},
};