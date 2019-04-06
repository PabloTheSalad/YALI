#ifndef SPECIALFORM_H
#define SPECIALFORM_H

#include "lispvalue.hpp"

std::pair<LispValue, bool> runSpecialForm(const LispValue&, const LispValue&);
LispValue defineSF(const LispValue& form, const LispValue& env, bool macro = false);
bool isSpecialForm(const LispString& str);

#endif // SPECIALFORM_H
