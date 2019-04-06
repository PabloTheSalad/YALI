#ifndef REPL_H
#define REPL_H

#include "lispvalue.hpp"

void repl(std::istream&, const LispValue&);
void loadFile(std::string filename, const LispValue& env, bool = true);

#endif // REPL_H
