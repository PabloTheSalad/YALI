#include "macro.h"
#include "exception.h"
#include "types/function.h"
#include "types/procedure.h"
#include "specialform.h"

#include <iostream>
#include <list>

std::pair<std::string, std::string> splitSpace(const std::string& str) {
    size_t space = str.find_first_of(' ');
    if (space == -1) return {str, ""};
    return {{str.begin(), str.begin()+space}, {str.begin()+space+1, str.end()}};
}

bool hasElipsis(const LispValue& val) {
    return val.list().rest().type == LispValue::List
            and val.list().rest().type == LispValue::List
            and !val.list().rest().isNull()
            and val.list().rest().list().front().type == LispValue::Atom
            and val.list().rest().list().front().atom() == "...";
}

std::pair<LispMap, bool> match(const LispVector& keywords, const LispValue& p, const LispValue& e) {
    LispValue pattern = p, expr = e;
    LispMap map;
    if (pattern.type == LispValue::Atom) {
        if (pattern.atom() == "...")
            throw patternError(pattern, "unexpected ellipsis");
        auto key = std::find(keywords.begin(), keywords.end(), pattern.atom());
        if (key != keywords.end()) {
            return {map, pattern == expr};
        } else {
            if (map.find(pattern.atom()+" ...") != map.end())
                throw patternError(pattern, "expected ellipsis");
            map[pattern.atom()] = expr;
            return {map, true};
        }
    } else if (pattern.type == LispValue::List and !pattern.isNull()) {
        if (hasElipsis(pattern)) {
            auto epattern = pattern.list().front();
            auto tail_pattern = pattern.list().rest().list().rest();
            bool p = false;
            LispMap emap;
            LispMap temp_map;
            while (true) {
                std::tie(temp_map, p) = match(keywords, tail_pattern, expr);
                if (p) {
                    map.insert(temp_map.begin(), temp_map.end());
                    map.insert(emap.begin(), emap.end());
                    return {map, true};
                } else {
                    if (expr.type != LispValue::List or expr.isNull()) return {map, false};
                    std::tie(temp_map, p) = match(keywords, epattern, expr.list().front());
                    if (p) {
                        for (auto node : temp_map) {
                            auto res = emap.find(node.first+" ...");
                            if (res == emap.end()) {
                                emap[node.first+" ..."] = PairList{node.second};
                            } else {
                                emap[node.first+" ..."].list().push_back(node.second);
                            }
                        }
                        expr = expr.list().rest();
                    } else return {map, false};
                }
            }
        } else {
            if (expr.type != LispValue::List or expr.isNull()) return {map, false};
            LispMap new_map;
            bool p;
            std::tie(new_map, p) = match(keywords, pattern.list().front(), expr.list().front());
            if (p) {
                map.insert(new_map.begin(), new_map.end());
                std::tie(new_map, p) = match(keywords, pattern.list().rest(), expr.list().rest());
                if (p) {
                    map.insert(new_map.begin(), new_map.end());
                    return {map, true};
                } else return {map, false};
            } else return {map, false};
        }
    } else {
        return {map, pattern == expr};
    }
}

//LispValue valueSubtitution(const LispMap& map, const LispValue& value) {
//    if (value.type != LispValue::List or value.isNull()) {
//        if (value.type == LispValue::Atom) {
//            auto find = map.find(value.atom());
//            if (find == map.end()) return value;
//            else return find->second;
//        } else return value;
//    } else {
//        auto car = valueSubtitution(map, value.list().front());
//        auto cdr = valueSubtitution(map, value.list().rest());
//        return PairList(car, cdr);
//    }
//}

LispVector findIdentifiers(const LispValue& expr) {
    if (expr.type == LispValue::Atom and expr.atom() != "...") {
        return {expr};
    } else if (expr.type == LispValue::List and !expr.isNull()) {
        auto icar = findIdentifiers(expr.list().front());
        auto icdr = findIdentifiers(expr.list().rest());
        std::move(icar.begin(), icar.end(), std::back_inserter(icdr));
        icdr.shrink_to_fit();
        return icdr;
    } else return {};
}

LispMap buildEllipsisMap(const LispValue& templ, const LispMap& map) {
    auto identifiers = findIdentifiers(templ);
    LispMap new_map;
    size_t ellipsis_var_length = 0;
    bool ellipsis_vars = false;
    for (auto id : identifiers) {
        auto varpair = map.find(id.atom());
        if (varpair != map.end()) {
            new_map.insert(*varpair);
        } else {
            varpair = map.find(id.atom()+" ...");
            if (varpair != map.end()) {
                auto len = varpair->second.list().size();
                if (ellipsis_vars and len != ellipsis_var_length)
                    throw patternError(templ, "different list length");
                else if (!ellipsis_vars) {
                    ellipsis_vars = true;
                    ellipsis_var_length = len;
                }
                new_map.insert(*varpair);
            }
        }
    }
//    if (!ellipsis_vars)
//        throw patternError(templ, "unexpected ellipsis");
    return new_map;
}

LispValue valueSubtitution(const LispMap& map, const LispValue& value);

LispValue valueSubtitutionRec(const LispMap& map, const LispValue& value) {
    if (value.type != LispValue::List or value.isNull()) {
        if (value.type == LispValue::Atom) {
            if (value.atom() == "...") throw patternError(value, "unexpected ellipsis");
            auto find = map.find(value.atom());
            if (map.find(value.atom()+" ...") != map.end())
                throw patternError(value, "expected ellipsis");
            if (find == map.end()) return value;
            else return find->second;
        } else return value;
    } else {
        if (hasElipsis(value)) {
            auto templ = value.list().front();
            auto tailmap = buildEllipsisMap(templ, map);
            LispMap headmap;
            LispValue new_list;
            bool p = true;
            bool duppled_wars;
            while (p) {
                duppled_wars = false;
                headmap.clear();
                for (auto& pair : tailmap) {
                    auto splitted_name = splitSpace(pair.first);
                    if (splitted_name.second == "...") {
                        if (tailmap[pair.first].isNull()) {
                            p = false;
                            break;
                        }
                        if (pair.second.list().front().type == LispValue::Atom
                                and pair.second.list().front().atom() == splitted_name.first) {
                            duppled_wars = true;
                        }
                        headmap[splitted_name.first] = pair.second.list().front();
                        tailmap[pair.first] = pair.second.list().rest();
                    } else {
                        headmap[pair.first] = pair.second;
                    }
                }
                if (!p) break;
                auto sub = valueSubtitutionRec(headmap, templ);
                if (sub == templ and !duppled_wars) break;
                if (new_list.isNull()) {
                    new_list = PairList{sub};
                } else {

                    new_list.list().push_back(sub);
                }
            }
            auto tail = value.list().rest().list().rest();
            if (new_list.isNull()) {
                new_list = valueSubtitutionRec(map, tail);
            } else {
                new_list.list().set_end(valueSubtitutionRec(map, tail));
            }
            return new_list;
        }
        auto car = valueSubtitutionRec(map, value.list().front());
        auto cdr = valueSubtitutionRec(map, value.list().rest());
        return PairList(car, cdr);
    }
}

LispVector findIdentifiers1(const LispValue& expr) {
    if (expr.type == LispValue::Atom and expr.atom() != "...") {
        return {expr};
    } else if (expr.type == LispValue::List and !expr.isNull()) {
        auto icar = findIdentifiers(expr.list().front());
        auto icdr = findIdentifiers(expr.list().rest());
        std::move(icar.begin(), icar.end(), std::back_inserter(icdr));
        icdr.shrink_to_fit();
        return icdr;
    } else return {};
}
static size_t counter = 0;
LispString genName() {
    counter++;
    return LispString("#Gen@#$:") + std::to_string(counter);
}

bool randomNameExist(const LispString& name) {
    auto i = name.find_first_of(':');
    bool p = LispString{name.begin(), name.begin()+i+1} == "#Gen@#$:";
    return p;
}

using RenameMap = std::unordered_map<LispString, LispString>;

//LispValue renameVars(LispMap& map, const LispValue& value, RenameMap& renamed) {
//    if (value.type == LispValue::Atom and value.atom() != "...") {
//        auto rename = renamed.find(value.atom());
//        if (rename != renamed.end()) {
//            return {LispAtom(rename->second), LispValue::Atom};
//        } else {
//            bool postfix = false;
//            auto res = map.find(value.atom());
//            if (res == map.end()) {
//                res = map.find(value.atom()+" ...");
//                postfix = true;
//                if (res == map.end()) return value;
//            }
//            LispString new_name = genName();
//            renamed.emplace(value.atom(), new_name);
//            LispValue value = res->second;
//            map.erase(res);
//            if (postfix)
//                map.emplace(new_name + " ...", value);
//            else
//                map.emplace(new_name, value);
//            return {LispAtom(new_name), LispValue::Atom};
//        }
//    } else if (value.type == LispValue::List and !value.isNull()) {
//        auto car = renameVars(map, value.list().front(), renamed);
//        auto cdr = renameVars(map, value.list().rest(), renamed);
//        return PairList(car, cdr);
//    } else {
//        return value;
//    }
//}

LispValue renameVars(const LispValue& value, RenameMap& renamed, const LispValue& env) {
    if (value.type == LispValue::Atom
            and value.atom() != "..."
            and value.atom() != "_") {
        auto rename = renamed.find(value.atom());
        if (rename != renamed.end()) {
            return {LispAtom(rename->second), LispValue::Atom};
        } else {
            LispString new_name;
            if (env.environment().hasName(value.atom())
                    or isSpecialForm(value.atom())
                    or randomNameExist(value.atom())) {
                new_name = value.atom();
            } else {
                new_name = genName();
            }
            renamed.emplace(value.atom(), new_name);
            return {LispAtom(new_name), LispValue::Atom};
        }
    } else if (value.type == LispValue::List and !value.isNull()) {
        if (value.list().front().type == LispValue::Atom
                and value.list().front().atom() == "quote")
            return value;
        auto car = renameVars(value.list().front(), renamed, env);
        auto cdr = renameVars(value.list().rest(), renamed, env);
        return PairList(car, cdr);
    } else {
        return value;
    }
}

LispValue valueSubtitution(const LispMap& map, const LispValue& value, const LispValue& env) {
    auto new_map = map;
    RenameMap renamed;
    for (auto& v : new_map) {
        v.second = renameVars(v.second, renamed, env);
    }
    return valueSubtitutionRec(new_map, value);
}

LispValue syntaxRules(const LispValue& form, const LispValue& env) {
    auto argsList = form.list().rest();
    if (argsList.list().front().type != LispValue::List) throw badSyntax(form);
    LispVector keywords;
    if (!argsList.list().front().isNull()) {
        for (auto key : argsList.list().front().list()) {
            if (key.type != LispValue::Atom) throw badSyntax(form);
            keywords.push_back(key);
        }
    }
    argsList = argsList.list().rest();
    if (argsList.isNull()) throw badSyntax(form);
    std::list<std::pair<LispValue, LispValue>> matchVector;
    for (auto pair : argsList.list()) {
        if (pair.type != LispValue::List or pair.list().size() != 2
                or pair.list().front().type != LispValue::List) throw badSyntax(form);
        matchVector.push_back({pair.list().front(), *std::next(pair.list().begin())});
    }
    auto func = [keywords, matchVector, env] (auto args) {
        auto value = args[0].list().rest();
        bool p;
        LispMap keymap;
        for (auto pair : matchVector) {
            std::tie(keymap, p) = match(keywords, pair.first.list().rest(), value);
            if (p) {
                return valueSubtitution(keymap, pair.second, env);
            }
        }
        throw patternError(args[0]);
    };
    return Function(func, 1, 1);
}

LispValue syntaxCase(const LispValue& form, const LispValue& env) {
    auto argsList = form.list().rest();
    auto expr = argsList.list().front().eval(env);
    if (expr.type != LispValue::List)
        throw patternError(expr);
    argsList = argsList.list().rest();
    LispVector keywords;
    if (!argsList.list().front().isNull()) {
        for (auto key : argsList.list().front().list()) {
            if (key.type != LispValue::Atom) throw badSyntax(form);
            keywords.push_back(key);
        }
    }
    argsList = argsList.list().rest();
    if (argsList.isNull()) throw badSyntax(form);
    std::list<std::tuple<LispValue, bool, LispValue>> matchVector;
    for (auto pair : argsList.list()) {
        auto size = pair.list().size();
        if (pair.type != LispValue::List or (size != 2 and size != 3)
                or pair.list().front().type != LispValue::List) throw badSyntax(form);
        auto pattern = pair.list().front();
        if (size == 2)
            matchVector.push_back({pattern, true, std::next(pair.list().begin())->eval(env)});
        else {
            auto guard = std::next(pair.list().begin())->eval(env).isTrue();
            auto value = std::next(pair.list().begin(), 2)->eval(env);
            matchVector.push_back({pattern, guard, value});
        }
    }
    bool p;
    LispMap keymap;
    LispValue pattern, value;
    bool guard;
    for (auto tuple : matchVector) {
        std::tie(pattern, guard, value) = tuple;
        std::tie(keymap, p) = match(keywords, pattern.list().rest(), expr.list().rest());
        if (p and guard) {
            return valueSubtitution(keymap, value, env).eval(env);
        }
    }
    throw patternError(expr);
}

std::pair<LispValue, bool> macroExpansionRec(const LispValue& val, const LispValue& mE) {
    auto macroEnv = mE;
    if ((val.type == LispValue::List and !val.isNull()
            and val.list().front().type == LispValue::Atom)
            or val.type == LispValue::Atom) {
        if (val.type == LispValue::List) {
            if (val.list().front().atom() == "begin") {
                macroEnv = Environment(macroEnv);
            } else if (val.list().front().atom() == "define-syntax") {
                macroEnv.environment().define(val.list().rest().list().front().atom(), {});
            }
        }
        LispValue macro;
        bool p;
        if (val.type == LispValue::List) {
            std::tie(macro, p) = macroEnv.environment().findMacro(val.list().front().atom());
        } else {
            std::tie(macro, p) = macroEnv.environment().findMacro(val.atom());
        }
        if (p) {
            LispVector args{val};
            if (macro.type == LispValue::Function) {
                macro = macro.function().apply(args);
            } else if (macro.type == LispValue::Procedure) {
                macro = macro.procedure().apply(args);
            } else throw badTypeFor("apply", macro);
            return {macro, true};
        }
    }
    bool p = false;
    if (val.type == LispValue::List and !val.isNull()) {
        bool p1 = false;
        auto list = val;
        while (list.list().rest().type == LispValue::List
               and !list.list().rest().isNull()) {
            std::tie(list.list().head, p1) = macroExpansionRec(list.list().head, macroEnv);
            p = p || p1;
            list = list.list().rest();
        }
        std::tie(list.list().head, p1) = macroExpansionRec(list.list().head, macroEnv);
        p = p || p1;
        std::tie(list.list().tail, p1) = macroExpansionRec(list.list().tail, macroEnv);
        p = p || p1;
    }
    return {val, p};
}

LispValue macroExpansion(const LispValue& val, const LispValue& macroEnv) {
    LispValue value = val;
    bool p = true;
    while (p) {
        std::tie(value, p) = macroExpansionRec(value, macroEnv);
        std::cout << "Macro expanded:" << std::endl << value << std::endl;
    }
    if (value.type == LispValue::List and !value.isNull()
            and value.list().front().type == LispValue::Atom) {
        if (value.list().front().atom() == "define-syntax") {
            defineSF(value, macroEnv);
        }
    }
    return value;
}
