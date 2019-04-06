#include "arithmetic.hpp"
#include <algorithm>
#include <numeric>
#include <float.h>
#include <cmath>
#include <limits>
#include "exception.hpp"
#include "types/number.hpp"

#define func(op) [](auto acc, auto a2) {return acc op a2.number();}

#define compare(op) \
    if (args.size() == 1) { \
        return true; \
    } else { \
        auto arg = args.begin(); \
        for (auto it = args.begin()+1; it != args.end(); it++) { \
            if (!(arg->number() op it->number())) return false; \
        else \
            arg = it; \
        } \
        return true; \
    }

LispValue sum(LispVector& args) {
    assertArgTypes("+", LispValue::Number, args);
    return std::accumulate(args.begin(), args.end(), Number(int64_t(0)), func(+));
}

LispValue sub(LispVector& args) {
    assertArgTypes("-", LispValue::Number, args);
    if (args.size() > 1)
        return std::accumulate(args.begin()+1, args.end(), args[0].number(), func(-));
    else
        return -args[0].number();
}

LispValue mult(LispVector& args) {
    assertArgTypes("*", LispValue::Number, args);
    return std::accumulate(args.begin(), args.end(), Number(int64_t(1)), func(*));
}

LispValue div(LispVector& args) {
    assertArgTypes("/", LispValue::Number, args);
    if (args.size() > 1) {
        return std::accumulate(args.begin()+1, args.end(), args[0].number(), func(/));
    } else
        return Number::fromInteger(1)/args[0].number();
}

LispValue gt(LispVector& args) {
    assertArgTypes(">", LispValue::Number, args);
    compare(>)
}

LispValue lt(LispVector& args) {
    assertArgTypes("<", LispValue::Number, args);
    compare(<)
}

LispValue gte(LispVector& args) {
    assertArgTypes(">=", LispValue::Number, args);
    compare(>=)
}

LispValue lte(LispVector& args) {
    assertArgTypes("<=", LispValue::Number, args);
    compare(<=)
}

LispValue eqnum(LispVector& args) {
    assertArgTypes("=", LispValue::Number, args);
    if (args.size() >= 1) {
        auto arg = args[0];
        for (u_long i = 1; i < args.size(); i++) {
            if (arg.number() != args[i].number()) return false;
        }
        return true;
    } else
        return true;
}

LispValue isRational(LispVector& args) {
    return args[0].type == LispValue::Number and args[0].number().isRational();
}

LispValue isInteger(LispVector& args) {
    return args[0].type == LispValue::Number and args[0].number().isInteger();
}

LispValue isReal(LispVector& args) {
    return args[0].type == LispValue::Number and args[0].number().isReal();
}

LispValue numerator(LispVector& args) {
    if (args[0].type != LispValue::Number or !args[0].number().isRational())
        throw badArgTypeName("numerator", args[0], 1, "Rational");
    return Number::fromInteger(args[0].number().getRational().getNumerator());
}

LispValue denominator(LispVector& args) {
    if (args[0].type != LispValue::Number or !args[0].number().isRational())
        throw badArgTypeName("numerator", args[0], 1, "Rational");
    return Number::fromInteger(args[0].number().getRational().getDenominator());
}

LispValue rationalize(LispVector& args) {
    assertArgType("rationalize", LispValue::Number, args[0], 1);
    if (args[1].type != LispValue::Number
            or args[1].number() < Number::fromInteger(0))
        throw badArgTypeName("rationalize", args[1], 2, "Positive number");
    return Number::fromRational(
                RatNum::fromReal(Number::numToReal(args[0].number()),
                                 Number::numToReal(args[1].number())));
}

LispValue abs(LispVector& args) {
    assertArgType("abs", LispValue::Number, args[0], 1);
    return Number::abs(args[0].number());
}

LispValue floor(LispVector& args) {
    assertArgType("floor", LispValue::Number, args[0], 1);
    return Number::fromInteger(std::floor(Number::numToReal(args[0].number())));
}

LispValue ceiling(LispVector& args) {
    assertArgType("ceiling", LispValue::Number, args[0], 1);
    return Number::fromInteger(std::ceil(Number::numToReal(args[0].number())));
}

LispValue truncate(LispVector& args) {
    assertArgType("truncate", LispValue::Number, args[0], 1);
    return Number::fromInteger(std::trunc(Number::numToReal(args[0].number())));
}

LispValue round(LispVector& args) {
    assertArgType("round", LispValue::Number, args[0], 1);
    return Number::fromInteger(std::round(Number::numToReal(args[0].number())));
}

LispValue remainder(LispVector& args) {
    assertArgTypes("remainder", LispValue::Number, args);
    if (!args[0].number().isInteger())
        throw badArgTypeName("remainder", args[0], 1, "Integer");
    if (!args[1].number().isInteger())
        throw badArgTypeName("remainder", args[1], 2, "Integer");
    return Number::fromInteger(std::remainder(args[0].number().getInteger(),
                                              args[1].number().getInteger()));
}

LispValue quotient(LispVector& args) {
    assertArgTypes("quotient", LispValue::Number, args);
    if (!args[0].number().isInteger())
        throw badArgTypeName("quotient", args[0], 1, "Integer");
    if (!args[1].number().isInteger())
        throw badArgTypeName("quotient", args[1], 2, "Integer");
    return Number::fromInteger(args[0].number().getInteger()
                              / args[1].number().getInteger());
}

LispValue modulo(LispVector& args) {
    assertArgTypes("quotient", LispValue::Number, args);
    if (!args[0].number().isInteger())
        throw badArgTypeName("quotient", args[0], 1, "Integer");
    if (!args[1].number().isInteger())
        throw badArgTypeName("quotient", args[1], 2, "Integer");
    auto mod = args[0].number().getInteger() % args[1].number().getInteger();
    if (mod < 0) mod = args[1].number().getInteger() + mod;
    return Number::fromInteger(mod);
}

LispValue expt(LispVector& args) {
    assertArgTypes("expt", LispValue::Number, args);
    if (args[0].number() < Number::fromInteger(0)) {
        throw badArgTypeName("expt", args[0], 1, "not negative number");
    }
    return Number::pow(args[0].number(), args[1].number());
}

bool oddGeneric(const char* funcName, LispVector& args) {
    assertArgTypes(funcName, LispValue::Number, args);
    if (!args[0].number().isInteger())
        throw badArgTypeName(funcName, args[0], 1, "Integer");
    return args[0].number().getInteger() % 2 == 1;
}

inline LispValue odd(LispVector& args) {
    return oddGeneric("odd?", args);
}

inline LispValue even(LispVector& args) {
    return !oddGeneric("even?", args);
}

LispValue max(LispVector& args) {
    LispValue max;
    count_t pos = 0;
    for (auto arg : args) {
        assertArgType("max", LispValue::Number, arg, pos);
        if (max.isNull() or max.number() < arg.number()) max = arg;
    }
    return max;
}

LispValue min(LispVector& args) {
    LispValue min;
    count_t pos = 0;
    for (auto arg : args) {
        assertArgType("max", LispValue::Number, arg, pos);
        if (min.isNull() or min.number() > arg.number()) min = arg;
    }
    return min;
}

LispValue sin(LispVector& args) {
    assertArgType("max", LispValue::Number, args[0], 1);
    return Number::fromReal(std::sin(Number::numToReal(args[0].number())));
}

LispValue cos(LispVector& args) {
    assertArgType("max", LispValue::Number, args[0], 1);
    return Number::fromReal(std::cos(Number::numToReal(args[0].number())));
}

LispValue tan(LispVector& args) {
    assertArgType("max", LispValue::Number, args[0], 1);
    return Number::fromReal(std::tan(Number::numToReal(args[0].number())));
}

LispValue asin(LispVector& args) {
    assertArgType("max", LispValue::Number, args[0], 1);
    return Number::fromReal(std::asin(Number::numToReal(args[0].number())));
}

LispValue acos(LispVector& args) {
    assertArgType("max", LispValue::Number, args[0], 1);
    return Number::fromReal(std::acos(Number::numToReal(args[0].number())));
}

LispValue atan(LispVector& args) {
    assertArgType("max", LispValue::Number, args[0], 1);
    return Number::fromReal(std::atan(Number::numToReal(args[0].number())));
}

LispValue exp(LispVector& args) {
    assertArgType("max", LispValue::Number, args[0], 1);
    return Number::fromReal(std::exp(Number::numToReal(args[0].number())));
}

LispValue log(LispVector& args) {
    assertArgType("max", LispValue::Number, args[0], 1);
    return Number::fromReal(std::log(Number::numToReal(args[0].number())));
}

LispValue nan(LispVector&) {
    return Number::fromReal(std::numeric_limits<long double>::signaling_NaN());
}

LispValue isNan(LispVector& args) {
    if (args[0].type == LispValue::Number and args[0].number().isReal()) {
        return std::isnan(args[0].number().getReal());
    } else {
        return false;
    }
}

LispValue inf(LispVector&) {
    return Number::fromReal(std::numeric_limits<long double>::infinity());
}

LispValue isInf(LispVector& args) {
    return args[0] == Number::fromReal(std::numeric_limits<long double>::infinity())
           || args[0] == Number::fromReal(-std::numeric_limits<long double>::infinity());
}

LispValue pi(LispVector&) {
    return Number::fromReal(3.1415926535897932384626433832795l);
}

LispMap Base::Ariphmetic::getMap() {
    return {
        {"+",           Function(sum,   0, MAX_COUNT)},
        {"-",           Function(sub,   1, MAX_COUNT)},
        {"*",           Function(mult,  0, MAX_COUNT)},
        {"/",           Function(div,   1, MAX_COUNT)},
        {">",           Function(gt,    0, MAX_COUNT)},
        {"<",           Function(lt,    0, MAX_COUNT)},
        {"<=",          Function(lte,   0, MAX_COUNT)},
        {">=",          Function(gte,   0, MAX_COUNT)},
        {"=",           Function(eqnum, 0, MAX_COUNT)},
        {"numerator",   Function(numerator, 1, 1)},
        {"denominator", Function(denominator, 1, 1)},
        {"integer?",    Function(isInteger, 1, 1)},
        {"real?",       Function(isReal, 1, 1)},
        {"rational?",   Function(isRational, 1, 1)},
        {"odd?",        Function(odd, 1, 1)},
        {"even?",       Function(even, 1, 1)},
        {"rationalize", Function(rationalize, 2, 2)},
        {"abs",         Function(abs, 1, 1)},
        {"floor",       Function(floor, 1, 1)},
        {"ceiling",     Function(ceiling, 1, 1)},
        {"truncate",    Function(truncate, 1, 1)},
        {"round",       Function(round, 1, 1)},
        {"remainder",   Function(remainder, 2, 2)},
        {"quotient",    Function(quotient, 2, 2)},
        {"modulo",      Function(modulo, 2, 2)},
        {"expt",        Function(expt, 2, 2)},
        {"max",         Function(max, 1, MAX_COUNT)},
        {"min",         Function(min, 1, MAX_COUNT)},
        {"sin",         Function(sin, 1, 1)},
        {"cos",         Function(cos, 1, 1)},
        {"tan",         Function(tan, 1, 1)},
        {"asin",        Function(asin, 1, 1)},
        {"acos",        Function(acos, 1, 1)},
        {"atan",        Function(atan, 1, 1)},
        {"exp",         Function(exp, 1, 1)},
        {"log",         Function(log, 1, 1)},
        {"pi",          Function(pi, 0, 0)},
        {"nan",         Function(nan, 0, 0)},
        {"nan?",        Function(isNan, 1, 1)},
        {"inf",         Function(inf, 0, 0)},
        {"inf?",        Function(isInf, 1, 1)},
    };
}
