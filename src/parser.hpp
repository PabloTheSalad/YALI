#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <iostream>
#include "lispvalue.hpp"
#include <vector>

LispValue parseExpr(std::istream&);
void skipSpaces(std::istream&);

#endif // INTERPRETER_H
