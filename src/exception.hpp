#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <stdexcept>
#include <sstream>
#include <string>
#include "lispvalue.hpp"

template <class T>
std::string toString(T obj) {
    std::ostringstream stream;
    stream << obj;
    return stream.str();
}

class LangException : public std::exception {
    std::string errorString;
    bool addObj = false;
public:
    LangException() : errorString() {}
    LangException(const LangException& e) = default;
    void setAddObj() { addObj = true; }
    bool getAddObj() { bool p = addObj; addObj = false; return p; }
    virtual const char* what() const noexcept;
    template <class T>
    LangException& operator << (T add) {
        errorString += toString(add);
        return *this;
    }
    void addMessage(const std::string& msg);
    ~LangException() {}
};

std::string printErrorObject(const LispValue& val);

LangException undefinedVariable(const LispValue& var);

LangException badTypeFor(const char* fun, const LispValue& var);

LangException badArgsValue(const LispValue& proc, size_t expected, size_t args2);

LangException badSyntax(const LispValue& var, bool = false);

void assertSyntax(const LispValue& var, bool p);

LangException badArg(const char* fun, const LispValue& arg,
                                size_t position, const char* msg = nullptr);

LangException badArgType(const char* fun, const LispValue& arg,
                                size_t position, decltype(LispValue::type) type,
                                const std::string& msg = "");

LangException badArgTypeName(const char* fun, const LispValue& arg,
                                size_t position, const std::string& type,
                                const std::string& msg = "");

void assertArgType(const char* fun, decltype(LispValue::type) type,
                   const LispValue& arg, size_t pos, const char* msg = nullptr);

void assertArgTypes(const char* fun, decltype(LispValue::type) type,
                    const LispVector& args);

void assertArgTypes(const char* fun,
                    std::vector<decltype(LispValue::type)> types,
                    const LispVector& args);

//LangException parseError(const Token& tok);
LangException badSpecialForm(const LispValue&);

LangException parseError(const std::string& symbol, const char* msg);

LangException userError(const std::string& msg, const LispValue& obj);

LangException userError(const std::string& msg);

LangException patternError(const LispValue&, const char* msg = nullptr);

LangException testError(const LispValue&);

LangException outOfRangeError(const char* fun, const LispNumber& n);

LangException divisionByZeroError(const char* fun);

#endif // EXCEPTION_H
