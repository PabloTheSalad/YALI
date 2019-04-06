#include "specialform.hpp"
#include "types/pairlist.hpp"
#include "types/environment.hpp"
#include "types/promise.hpp"
#include "types/procedure.hpp"
#include "exception.hpp"
#include "macro.hpp"
#include <unordered_set>

LispValue defineSF(const LispValue& form, const LispValue& env, bool macro) {
    auto arg = form.list().rest().list().begin();
    auto& name = arg->atom();
    auto val = (++arg)->eval(env);
    env.environment().define(name, val, macro);
    return undefined();
}

LispValue setSF(const LispValue& form, const LispValue& env) {
    auto arg = form.list().rest().list().begin();
    if (arg->type != LispValue::Atom) throw badSpecialForm(form);

    auto& name = arg->atom();
    ++arg;
    if (!arg.get().list().tail.isNull()) throw badSpecialForm(form);
    auto val = arg->eval(env);
    env.environment().set(name, val);
    return undefined();
}

LispValue lambdaSF(const LispValue& form, const LispValue& env) {
    auto args = form.list().rest().list().begin();
    auto argsList = *args;
    args++;
    LispVector argsVec;
    LispValue valarg;
    if (!argsList.isNull() and argsList.type == LispValue::List) {
        auto it = argsList.list().begin();
        for (; !(it.get().isNull()
             or it.get().type != LispValue::List); it++) {
            argsVec.push_back(*it);
        }
        valarg = it.get().type == LispValue::List ? LispValue{} : it.get();
    } else if (argsList.type != LispValue::List) {
        valarg = argsList;
    }
    auto body = *args;
    return Procedure(argsVec, body, env, valarg);
}

LispValue ifSF(const LispValue& form, const LispValue& env) {
    auto args = form.list().rest();
    if (auto s = form.list().size()-1; s < 2 or s > 3) throw badSpecialForm(form);
    auto p = args.list().begin();
    if (p->eval(env).isTrue()) {
        return std::next(p)->eval(env);
    } else {
        auto arg = std::next(p, 2);
        if (arg.get().isNull()) return {};
        else return arg->eval(env);
    }
}

LispValue evlisSF(const LispValue& form, const LispValue& env) {
    auto argsList = form.list().rest();
    if (!argsList.isNull()) {
        auto it = argsList.list().begin();
        LispValue res;
        while (true) {
            if (it.get().isNull()) {
                break;
            } else if (it.get().type != LispValue::List) {
                throw badSpecialForm(form);
            }
            res = it->eval(env);
            it++;
        }
        return res;
    } else return undefined();
}

LispValue doSF(const LispValue& form, const LispValue& env) {
    auto argsList = form.list().rest();
    if (argsList.isNull()
            or argsList.type != LispValue::List
            or argsList.list().size() < 2)
        throw badSpecialForm(form);
    auto iterators = argsList.list().front();
    if (iterators.type != LispValue::List)
        throw badSpecialForm(form);
    LispValue new_env = Environment(env);
    std::vector<std::pair<LispAtom, LispValue>> updates;
    while(!iterators.isNull()) {
        auto& iter = iterators.list().front();
        if (iter.isNull()
                or iter.type != LispValue::List
                or iter.list().size() != 3
                or iter.list().front().type != LispValue::Atom)
            throw badSpecialForm(form);

        new_env.environment().define(iter.list().front().atom(),
                                     iter.list().rest().list().front().eval(env));
        updates.push_back({iter.list().front().atom(),
                          iter.list().rest().list().rest().list().front()});
        iterators = iterators.list().rest();
    }
    auto pred = argsList.list().rest().list().front();
    if (pred.isNull() or pred.type != LispValue::List or pred.list().size() != 2)
        throw badSpecialForm(form);
    LispValue body = PairList({"evlis", LispValue::Atom},argsList.list().rest().list().rest());
    while(!pred.list().front().eval(new_env).isTrue()) {
        body.eval(new_env);
        for (auto u : updates) {
            new_env.environment().define(u.first, u.second.eval(new_env));
        }
    }
    return pred.list().rest().list().front().eval(new_env);
}

LispValue beginSF(const LispValue& form, const LispValue& env) {
    LispValue new_env = Environment(env);
    return evlisSF(form, new_env);
}

LispValue andPredSF(const LispValue& form, const LispValue& env) {
    for (auto arg : form.list().rest().list()) {
        auto evaled = arg.eval(env);
        if (!evaled.isTrue()) return evaled;
    }
    return true;
}

LispValue orPredSF(const LispValue& form, const LispValue& env) {
    for (auto arg : form.list().rest().list()) {
        auto evaled = arg.eval(env);
        if (evaled.isTrue()) return evaled;
    }
    return false;
}

LispValue delaySF(const LispValue& form, const LispValue&) {
    auto argsList = form.list().rest();
    if (argsList.type != LispValue::List or !argsList.list().rest().isNull()) {
        throw badSpecialForm(form);
    }
    return LispPromise(argsList.list().front());
}

LispValue forceSF(const LispValue& form, const LispValue& env) {
    auto argsList = form.list().rest();
    if (argsList.type != LispValue::List
            or !argsList.list().rest().isNull()) {
        throw badSpecialForm(form);
    }
    auto promise = argsList.list().front().eval(env);
    if (promise.type != LispValue::Promise) {
        badArgType("force", argsList.list().front(), 1, LispValue::Promise);
    }
    return promise.promise().eval(env);
}

const std::unordered_set<LispString> specialForms = {
    "defineSF@YALI",
    "define-syntax",
    "set!",
    "lambdaSF@YALI",
    "begin",
    "evlis",
    "ifSF@YALI",
    "quote",
    "syntax-case",
    "syntax",
    "and",
    "or",
    "do",
    "delay",
    "force",
};

bool isSpecialForm(const LispString& s) {
    return specialForms.find(s) != specialForms.end();
}

std::pair<LispValue, bool> runSpecialForm(const LispValue& form, const LispValue& env) {
    if (form.list().front().type != LispValue::Atom) return {{}, false};
    auto& name = form.list().front().atom();
    if (name == "defineSF@YALI")      return {defineSF(form, env), true};
    else if (name == "define-syntax") return {defineSF(form, env, true), true};
    else if (name == "set!")          return {setSF(form, env), true};
    else if (name == "lambdaSF@YALI") return {lambdaSF(form, env), true};
    else if (name == "begin")         return {beginSF(form, env), true};
    else if (name == "evlis")         return {evlisSF(form, env), true};
    else if (name == "ifSF@YALI")     return {ifSF(form, env), true};
    else if (name == "quote")         return {*std::next(form.list().begin()), true};
    else if (name == "syntax-case")   return {syntaxCase(form, env), true};
    else if (name == "syntax")        {
        auto val = form.list().rest().list().front();
//        val.setSyntax(true);
        return {val, true};
    }
    else if (name == "and") return {andPredSF(form, env), true};
    else if (name == "or") return {orPredSF(form, env), true};
    else if (name == "do") return {doSF(form, env), true};
    else if (name == "delay") return {delaySF(form, env), true};
    else if (name == "force") return {forceSF(form, env), true};
    else return {{}, false};
}
