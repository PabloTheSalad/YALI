#include "environment.hpp"
#include "exception.hpp"
#include <iterator>
#include <algorithm>

void Environment::add_values(LispMap&& map) {
    for (auto v : map) {
        values.emplace(v.first, std::make_pair(v.second, false));
    }
}


void Environment::define(const LispString& name, const LispValue& value, bool macro) {
        values[name] = {value, macro};
}

void Environment::set(const LispString& name, const LispValue& value) {
    auto res = values.find(name);
    if (res != values.end())
        res->second = {value, false};
    else if (!outer.isNull())
        outer.environment().set(name, value);
    else
        throw undefinedVariable(name);
}

bool Environment::hasName(const LispString& str) const {
    if (values.find(str) != values.end())
        return true;
    else if (!outer.isNull())
        return outer.environment().hasName(str);
    else
        return false;
}

LispValue Environment::find(const LispString& name) const {
    auto res = values.find(name);
    if (res != values.end())
        return res->second.first;
    else if (!outer.isNull())
        return outer.environment().find(name);
    else
        throw undefinedVariable(name);
}

std::pair<LispValue, bool> Environment::findMacro(const LispString& name) const {
    auto res = values.find(name);
    if (res != values.end() and res->second.second)
        return {res->second.first, true};
    else if (!outer.isNull())
        return outer.environment().findMacro(name);
    else
        return {{}, false};
}
