#ifndef FUNCTION_H
#define FUNCTION_H

#include "lispvalue.hpp"
#include <functional>

using PrimitiveFunction = std::function<LispValue(LispVector&)>;

class Function {
    PrimitiveFunction func;
    count_t minArgs;
    count_t maxArgs;
    friend std::ostream& operator << (std::ostream& out, const LispValue& val);
public:
    LispValue apply(LispVector&) const;
    Function(PrimitiveFunction f, size_t min, size_t max)
        : func(f), minArgs(min), maxArgs(max) {}
    Function(LispValue(*f)(LispVector&), size_t min, size_t max)
        : func(f), minArgs(min), maxArgs(max) {}
    Function(const Function&) = default;
    Function& operator = (const Function&) = default;
    Function(Function&&) = default;
    Function& operator = (Function&&) = default;
    inline bool operator == (const Function& f) const { return false; }
};

#endif // FUNCTION_H
