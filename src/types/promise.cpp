#include "promise.hpp"

LispValue LispPromise::eval(const LispValue& env) {
    if (!force) {
        value = value.eval(env);
        force = true;
    }
    return value;
}
