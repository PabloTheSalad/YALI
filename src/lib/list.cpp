#include "list.hpp"
#include "../types/pairlist.hpp"
#include "exception.hpp"
#include "macro.hpp"

#include <iostream>

LispValue car(LispVector& args) {
    if (args[0].type != LispValue::List or args[0].isNull())
        throw badArgType("car", args[0], 1, LispValue::List);
    return args[0].list().front();
}

LispValue cdr(LispVector& args) {
    if (args[0].type != LispValue::List or args[0].isNull())
        throw badArgType("cdr", args[0], 1, LispValue::List);
    return args[0].list().rest();
}

LispValue cons(LispVector& args) {
    return PairList(args[0], args[1]);
}

LispValue test(LispVector& args) {
    if (args[0].type != LispValue::String)
        throw badArgType("test", args[0], 1, LispValue::String);
    else if (args[1] != args[2])
        throw testError(args[0]);
    else
        std::cout << "Test " << args[0] << " passed" << std::endl;
        return {};
}

LispMap Base::List::getMap() {
    return {
        {"car", Function(car, 1, 1)},
        {"cdr", Function(cdr, 1, 1)},
        {"cons", Function(cons, 2, 2)},
        {"test", Function(test, 3, 3)}
    };
}
