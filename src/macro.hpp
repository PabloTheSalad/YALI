#ifndef MACRO_H
#define MACRO_H

#include "lispvalue.hpp"
#include "types/environment.hpp"
#include "types/pairlist.hpp"
#include <tuple>
#include <algorithm>

std::pair<LispMap, bool> match(const LispVector& keywords, const LispValue& pattern, const LispValue& expr);
LispValue syntaxRules(const LispValue& form, const LispValue&);
LispValue syntaxCase(const LispValue& form, const LispValue& env);
LispValue macroExpansion(const LispValue& val, const LispValue& macroEnv);

#endif // MACRO_H
