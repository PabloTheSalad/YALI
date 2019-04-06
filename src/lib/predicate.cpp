#include "predicate.hpp"
#include "../types/pairlist.hpp"
#include "../types/promise.hpp"
#include "exception.hpp"
#include "macro.hpp"

#include <algorithm>
#include <numeric>
#include <float.h>

#define func(op) [](auto acc, auto a2) {return acc op a2.number();}

LispValue isNull(LispVector& args) {
    return args[0].isNull();
}

LispValue identifier(LispVector& args) {
    return args[0].type == LispValue::Atom;
}

LispValue eq(LispVector& args) {
    if (args.size() == 0) return true;
    auto i = std::accumulate(args.begin(), args.end(), args[0].getPtr(),
                           [](auto acc, auto a2) {
        return acc == a2.getPtr() ? acc : nullptr;
    });
    return i == nullptr ? false : true;
}

LispValue eqv(LispVector& args) {
    if (args.size() != 0) {
        auto pred = args[0];
        for (auto arg = args.begin()+1; arg != args.end(); arg++) {
            if (pred.type == LispValue::List or pred.type == LispValue::Vector) {
                if (pred.getPtr() != arg->getPtr()) return false;
            } else {
                if (pred != *arg) return false;
            }
        }
    }
    return true;
}

LispValue equal(LispVector& args) {
    if (args.size() != 0) {
        auto pred = args[0];
        for (auto arg = args.begin()+1; arg != args.end(); arg++) {
            if (pred != *arg) return false;
        }
    }
    return true;
}

LispValue isBoolean(LispVector& args) {
    return args[0].type == LispValue::Bool;
}

LispValue isSymbol(LispVector& args) {
    return args[0].type == LispValue::Atom;
}

LispValue isNumber(LispVector& args) {
    return args[0].type == LispValue::Number;
}

LispValue isChar(LispVector& args) {
    return args[0].type == LispValue::Character;
}

LispValue isString(LispVector& args) {
    return args[0].type == LispValue::String;
}

LispValue isVector(LispVector& args) {
    return args[0].type == LispValue::Vector;
}

LispValue isProcedure(LispVector& args) {
    return args[0].type == LispValue::Procedure
            or args[0].type == LispValue::Function;
}

LispValue isPair(LispVector& args) {
    return args[0].type == LispValue::List;
}

LispValue isPort(LispVector& args) {
    return args[0].type == LispValue::Port;
}

LispValue notProc(LispVector& args) {
    return not args[0].isTrue();
}


LispMap Base::Predicate::getMap() {
    return {
        {"null?",           Function(isNull, 1, 1)},
        {"identifier?",     Function(identifier, 1, 1)},
        {"eq?",             Function(eq, 0, MAX_COUNT)},
        {"eqv?",            Function(eqv, 0, MAX_COUNT)},
        {"equal?",          Function(equal, 0, MAX_COUNT)},
        {"boolean?",        Function(isBoolean, 1, 1)},
        {"symbol?",         Function(isSymbol, 1, 1)},
        {"number?",         Function(isNumber, 1, 1)},
        {"char?",           Function(isChar, 1, 1)},
        {"string?",         Function(isString, 1, 1)},
        {"vector?",         Function(isVector, 1, 1)},
        {"procedure?",      Function(isProcedure, 1, 1)},
        {"pair?",           Function(isPair, 1, 1)},
        {"port?",           Function(isPort, 1, 1)},
        {"not",             Function(notProc, 1, 1)}
    };
}

LispValue isPromise(LispVector& args) {
    return args[0].type == LispValue::Promise;
}

LispValue isPromiseForced(LispVector& args) {
    assertArgTypes("promise-forced?", LispValue::Promise, args);
    return args[0].promise().forced();
}

LispValue promiseValue(LispVector& args) {
    assertArgTypes("promise-value", LispValue::Promise, args);
    if (!args[0].promise().forced()) {
        throw badArgTypeName("promise-value", args[0], 1, "forced Promise");
    }
    return args[0].promise().get();
}

LispMap Base::Promise::getMap() {
    return {
        {"promise?",        Function(isPromise, 1, 1)},
        {"promise-forced?",  Function(isPromiseForced, 1, 1)},
        {"promise-value",   Function(promiseValue, 1, 1)},
    };
}
