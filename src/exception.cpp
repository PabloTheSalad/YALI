#include "exception.hpp"
#include "types/pairlist.hpp"
//#include "tokenizer.h"

const char* LangException::what() const noexcept {
    return errorString.c_str();
}

void LangException::addMessage(const std::string& msg) {
    if (!msg.empty()){
        errorString += "\nMessage: ";
        errorString += msg;
    }
}

std::string printErrorObject(const LispValue& val) {
    return "[error object: " + toString(val) + "]";
}

LangException undefinedVariable(const LispValue& var) {
    LangException err;
    err << "NameError: Undefined variable: " << var;
    return err;
}

LangException badTypeFor(const char* fun, const LispValue& var) {
    LangException err;
    err << "TypeError: Bad type for " << fun << " " << printErrorObject(var);
    return err;
}

LangException badArgsValue(const LispValue& proc, size_t expected, size_t args2) {
    LangException err;
    err << "TypeError: bad value of args " << args2 << " for " << proc << " expected " << expected;
    return err;
}

LangException badSyntax(const LispValue& var, bool obj) {
    LangException err;
    err << "SyntaxError: Bad syntax in expression: " << var;
    if (obj) err.setAddObj();
    return err;
}

void assertSyntax(const LispValue& var, bool p) {
    if (!p) {
        throw badSyntax(var);
    }
}

LangException badArg(const char* fun, const LispValue& arg,
                                size_t position, const char* msg) {
    LangException err;
    err << "TypeError: In function \"" << fun << "\" Bad argument "
        << "on position " << position << " " << ": " << arg;
    err.addMessage(msg);
    return err;
}

LangException badArgType(const char* fun, const LispValue& arg,
                                size_t position, decltype(LispValue::type) type,
                                const std::string& msg) {
    LangException err;
    err << "TypeError: In function \"" << fun << "\" Bad argument type "
        << "on position " << position << " " << ": " << arg
        << " , expected " << typeName(type);
    err.addMessage(msg);
    return err;
}

LangException badArgTypeName(const char* fun, const LispValue& arg,
                                size_t position, const std::string& type,
                                const std::string& msg) {
    LangException err;
    err << "TypeError: In function \"" << fun << "\" Bad argument type "
        << "on position " << position << " " << ": " << arg
        << " , expected " << type;
    err.addMessage(msg);
    return err;
}

void assertArgType(const char* fun, decltype(LispValue::type) type,
                   const LispValue& arg, size_t pos, const char* msg) {
    if (arg.type != type)
        throw badArgType(fun, arg, pos, type, msg);
}

void assertArgTypes(const char* fun, decltype(LispValue::type) type, const LispVector& args) {
    size_t pos = 1;
    for (auto arg : args) {
        if (arg.type != type) {
            throw badArgType(fun, arg, pos, type);
        }
        pos++;
    }
}

void assertArgTypes(const char* fun, std::vector<decltype(LispValue::type)> types, const LispVector& args) {
    auto typeIt = types.begin();
    size_t pos = 1;
    for (auto arg : args) {
        if (arg.type != *typeIt) {
            throw badArgType(fun, arg, pos, *typeIt);
        }
        typeIt++;
        pos++;
    }
}

//LangException parseError(const Token& tok) {
//    LangException err;
//    err << "ParseError: unexpected token: '" << tok << "'";
//    return err;
//}

LangException badSpecialForm(const LispValue& form) {
    LangException err;
    err << "SyntaxError: ill-formed "
        << ((form.type == LispValue::List and !form.isNull()) ? form.list().front() : form)
        << " special form in expression: " << form;
    return err;
}

LangException parseError(const std::string& symbol, const char* msg) {
    LangException err;
    err << "ParseError: unexpected symbol '" << symbol << "'";
    err.addMessage(msg);
    return err;
}

LangException userError(const std::string& msg, const LispValue& obj) {
    LangException err;
    err << "UserError: " << msg << " " << obj;
    return err;
}

LangException userError(const std::string& msg) {
    LangException err;
    err << "UserError: " << msg;
    return err;
}

LangException patternError(const LispValue& val, const char* msg) {
    LangException err;
    err << "Pattern matching error in " << val;
    if (msg != nullptr) err.addMessage(msg);
    return err;
}

LangException testError(const LispValue& name) {
    LangException err;
    err << "TestErrror: Test " << name << " not passed!";
    return err;
}

LangException outOfRangeError(const char* fun, const LispNumber& n) {
    LangException err;
    err << "In procedure " << fun << ": Value out of range: " << n;
    return err;
}

LangException divisionByZeroError(const char* fun) {
    LangException err;
    err.setAddObj();
    err << "ZeroDivisionError: In procedure " << fun;
    return err;
}
