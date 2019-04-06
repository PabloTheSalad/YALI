#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "lispvalue.hpp"
#include <unordered_map>

using LispMap = std::unordered_map<LispString, LispValue>;
using LispMacroMap = std::unordered_map<LispString, std::pair<LispValue, bool>>;
using RenameMap = std::unordered_map<LispString, LispString>;

class Environment {
    LispMacroMap values;
    LispValue outer;
public:
    Environment() : values(), outer() {}
    Environment(const LispValue& v) : values(), outer(v) {}
    void add_values(LispMap&&);
    const LispMacroMap& getValues() const { return values; }
    void delOuter() { outer = {}; }
    void define(const LispString&, const LispValue&, bool macro = false);
    void set(const LispString&, const LispValue&);
    bool hasName(const LispString&) const;
    LispValue find(const LispString&) const;
    std::pair<LispValue, bool>  findMacro(const LispString&) const;
    inline bool operator == (const Environment& e) const {
        return values == e.values;
    }
};

#endif // ENVIRONMENT_H
