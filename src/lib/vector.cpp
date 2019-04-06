#include "vector.hpp"
#include "exception.hpp"

LispValue vector(LispVector& args) {
    return args;
}

LispValue makeVector(LispVector& args) {
    if (args[0].type != LispValue::Number or !args[0].number().isInteger())
        throw badArgTypeName("make-vector", args[0], 1, "Integer");
    if (args[0].number().getInteger() < 0)
        throw outOfRangeError("make-vector", args[0].number());
    if (args.size() == 1) {
        return LispVector(args[0].number().getInteger());
    } else {
        return LispVector(args[0].number().getInteger(), args[1]);
    }
}

LispValue vectorLength(LispVector& args) {
    assertArgType("vector-length", LispValue::Vector, args[0], 1);
    return LispNumber(Number::fromInteger(args[0].vector().size()));
}

LispValue vectorRef(LispVector& args) {
    assertArgType("vector-ref", LispValue::Vector, args[0], 1);
    assertArgType("vector-ref", LispValue::Number, args[1], 2);
    if (!args[1].number().isInteger())
        throw badArgTypeName("vector-ref", args[1], 2, "Integer");
    if (args[1].number() >= Number::fromInteger(args[0].vector().size())
            or args[1].number().getInteger() < 0)
        throw outOfRangeError("vector-ref", args[1].number());
    return args[0].vector()[args[1].number().getInteger()];
}

LispValue vectorSet(LispVector& args) {
    assertArgType("vector-set!", LispValue::Vector, args[0], 1);
    assertArgType("vector-set!", LispValue::Number, args[1], 2);
    if (!args[1].number().isInteger())
        throw badArgTypeName("vector-ref", args[1], 2, "Integer");
    if (args[1].number() >= Number::fromInteger(args[0].vector().size())
            or args[1].number().getInteger() < 0)
        throw outOfRangeError("vector-ref", args[1].number());
    else {
        args[0].vector()[args[1].number().getInteger()] = args[2];
        return {};
    }
}

LispMap Base::Vector::getMap() {
    return {
        {"vector", Function(vector, 0, MAX_COUNT)},
        {"make-vector", Function(makeVector, 1, 2)},
        {"vector-length", Function(vectorLength, 1, 1)},
        {"vector-ref", Function(vectorRef, 2, 2)},
        {"vector-set!", Function(vectorSet, 3, 3)},
    };
}
