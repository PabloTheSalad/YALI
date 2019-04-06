#ifndef LISPTYPE_H
#define LISPTYPE_H

#include "counter.hpp"
#include "types/number.hpp"
#include "types/port.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <strstream>

class LispValue;
using LispNumber = class Number;
using LispBool = bool;
using LispAtom = std::string;
using LispCharacter = char;
using LispString = std::string;
using LispList = struct PairList;
using LispVector = std::vector<LispValue>;
using LispFunction = class Function;
using LispProcedure = class Procedure;
using LispEnvironment = class Environment;
class LispPromise;

class LispValue {
    union {
        void* value = nullptr;
        LispCharacter mcharacter;
        LispBool mboolean;
    };

    RefCounter counter;
    friend std::ostream& operator << (std::ostream&, const LispValue&);
    friend LispValue undefined();
public:
    enum {
        Number,
        Bool,
        Atom,
        Character,
        String,
        List,
        Vector,
        Function,
        Procedure,
        Environment,
        Port,
        Promise,
        Undef,
    } type;
    LispValue() : value(nullptr), type(List) {}
    LispValue(const LispValue&) = default;
    LispValue& operator = (const LispValue&) = default;
    LispValue(LispValue&&) = default;
    LispValue& operator = (LispValue&& v);

    LispValue(LispNumber&&);
    LispValue(LispBool&&);
    LispValue(LispAtom&&, decltype(type) = Atom);
    LispValue(LispCharacter&&);
    LispValue(LispString&&);
    LispValue(const LispString&);
    LispValue(LispList&&);
    LispValue(const LispList&);
    LispValue(LispVector&&);
    LispValue(const LispVector&);
    LispValue(LispFunction&&);
    LispValue(const LispFunction&);
    LispValue(LispProcedure&&);
    LispValue(const LispProcedure&);
    LispValue(LispEnvironment&&);
    LispValue(LispPort*);
    LispValue(const LispPromise&);

    ~LispValue();

    bool operator == (const LispValue&) const;
    bool operator != (const LispValue&) const;

    void* getPtr() const { return value; }

    template<class T>
    inline T& getValue() const { return *static_cast<T*>(value); }
    inline const LispNumber& number() const       { return getValue<LispNumber>(); }
    inline LispBool boolean() const               { return mboolean; }
    inline LispAtom& atom() const           { return getValue<LispAtom>(); }
    inline const LispString& string() const       { return getValue<LispString>(); }
    inline const LispCharacter& character() const { return mcharacter; }
    LispList& list() const;
    inline LispVector& vector() const             { return getValue<LispVector>(); }
    const LispFunction& function() const;
    LispProcedure& procedure() const;
    LispEnvironment& environment() const;
    LispPort& port() const { return getValue<LispPort>(); }
    LispPromise& promise() const;

    inline bool isNull() const { return type == List and value == nullptr; }
    inline bool isTrue() const { return type != Bool or boolean(); }
    inline void setSyntax(bool) const {}
    bool syntax() const {return false;}

    LispValue eval(const LispValue&) const;
    LispValue toString() const {
        std::strstream stream;
        stream << *this;
        return stream.str();
    }
};

inline LispValue undefined() {
    LispValue v;
    v.type = LispValue::Undef;
    return v;
}
const char* typeName(decltype(LispValue::type));
std::ostream& operator << (std::ostream& out, const LispValue& val);

#endif // LISPTYPE_H
