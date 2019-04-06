#ifndef PROCEDURE_H
#define PROCEDURE_H

#include "lispvalue.hpp"

class Procedure {
    LispVector args;
    LispValue valarg;
    LispValue body;
    LispValue env;
    friend std::ostream& operator << (std::ostream& out, const LispValue& val);
public:
    Procedure(const Procedure&) = default;
    Procedure& operator = (const Procedure&) = default;
    Procedure(Procedure&&) = default;
    Procedure& operator = (Procedure&&) = default;
    Procedure(const LispVector& a, const LispValue& b, const LispValue& e, const LispValue v = {})
        : args(a), valarg(v), body(b), env(e) {}

    inline bool operator == (const Procedure&) const { return false; }

    LispValue apply(LispVector&);
};

#endif // PROCEDURE_H
