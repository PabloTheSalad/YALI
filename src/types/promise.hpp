#ifndef PROMISE_HPP
#define PROMISE_HPP

#include "lispvalue.hpp"

class LispPromise {
    LispValue value;
    bool force = false;
public:
    LispPromise(const LispPromise&) = default;
    LispPromise(const LispValue& val) : value(val) {}
    LispValue get() const { return value; }
    bool forced() const { return force; }
    LispValue eval(const LispValue&);
};

#endif // PROMISE_HPP
