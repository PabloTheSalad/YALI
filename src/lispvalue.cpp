#include <iostream>
#include <unordered_set>
#include <fstream>

#include "lispvalue.hpp"
#include "types/pairlist.hpp"
#include "types/environment.hpp"
#include "types/procedure.hpp"
#include "types/function.hpp"
#include "types/number.hpp"
#include "types/promise.hpp"
#include "specialform.hpp"
#include "exception.hpp"
#include "datapool.hpp"

LispPort* LispPort::openFile(const LispString& fn, bool in) {
    auto port = new LispPort;
    if (in) {
        port->stream = (std::iostream*) new std::ifstream(fn);
    } else {
        port->stream = (std::iostream*) new std::ofstream(fn);
    }
    port->input = in;
    return port;
}



LispValue& LispValue::operator = (LispValue&& v) {
    std::swap(counter, v.counter);
    std::swap(value, v.value);
    std::swap(type, v.type);
    return *this;
}

//template <>
//struct LispValue::Cell<PairList> {
//    bool syntax = false;
//    PairList value;
//};

LispList& LispValue::list() const               { return getValue<LispList>(); }
const LispFunction& LispValue::function() const { return getValue<LispFunction>(); }
LispProcedure& LispValue::procedure() const     { return getValue<LispProcedure>(); }
LispEnvironment& LispValue::environment() const { return getValue<LispEnvironment>(); }
LispPromise& LispValue::promise() const { return getValue<LispPromise>(); }

LispValue::LispValue(LispNumber&& n) : type(Number) {
    value = numberPool.findOrAdd(n);
//    counters.numbers++;
}

LispValue::LispValue(LispBool&& b) : type(Bool) {
    mboolean = b;
}

LispValue::LispValue(LispAtom&& a, decltype(type) t) : type(t) {
//    value = new LispAtom(a);
    value = stringPool.findOrAdd(a);
}

LispValue::LispValue(LispCharacter&& c) : type(Character) {
    mcharacter = c;
}

LispValue::LispValue(LispString&& s) : type(String) {
//    value = new LispString(s);
    value = stringPool.findOrAdd(s);
}

LispValue::LispValue(const LispString& s) : type(String) {
//    value = new LispString(s);
    value = stringPool.findOrAdd(s);
}

LispValue::LispValue(LispList&& l) : type(List) {
    value = new LispList(l);
}

LispValue::LispValue(const LispList& l) : type(List) {
    value = new LispList(l);
}

LispValue::LispValue(LispVector&& v) : type(Vector) {
    value = new LispVector(v);
}

LispValue::LispValue(const LispVector& v) : type(Vector) {
    value = new LispVector(v);
}

LispValue::LispValue(LispFunction&& f) : type(Function) {
    value = new LispFunction(f);
}

LispValue::LispValue(const LispFunction& f) : type(Function) {
    value = new LispFunction(f);
}

LispValue::LispValue(LispProcedure&& p) : type(Procedure) {
    value = new LispProcedure(p);
}

LispValue::LispValue(const LispProcedure& p) : type(Procedure) {
    value = new LispProcedure(p);
}

LispValue::LispValue(LispEnvironment&& p) : type(Environment) {
    value = new LispEnvironment(p);
}

LispValue::LispValue(LispPort* p) : type(Port) {
    filePool.insert(p);
    value = p;
}

LispValue::LispValue(const LispPromise& p) : type(Promise) {
    value = new LispPromise(p);
}

LispValue::~LispValue() {
    if (!counter.empty() and counter.count() == 1) {
/*        if (type == Atom or type == String)
            delete static_cast<LispAtom*>(value);
        else*/ /*if (type == Number) {
            delete static_cast<LispNumber*>(value);
//            counters.numbers--;
        } else*/ if (type == Procedure) {
            delete  static_cast<LispProcedure*>(value);
//            counters.procs--;
        } else if (type == Function) {
            delete  static_cast<LispFunction*>(value);
//            counters.funcs--;
        } else if (type == Environment) {
            delete  static_cast<LispEnvironment*>(value);
//            counters.envs--;
        } else if (type == List) {
            delete  static_cast<LispList*>(value);
//            counters.lists--;
        } else if (type == Vector) {
            delete static_cast<LispVector*>(value);
//            counters.vectors--;
        } else if (type == Port) {
            filePool.erase_ptr(static_cast<LispPort*>(value));
        } else if (type == Promise) {
            delete static_cast<LispPort*>(value);
        }
    }
}

bool LispValue::operator == (const LispValue& o) const {
    if (type == o.type) {
        if (type == Atom or type == String) {
            return atom() == o.atom();
        } else if (type == Number) {
            return number() == o.number();
        } else if (type == Bool) {
            return boolean() == o.boolean();
        } else if (type == Character) {
            return character() == o.character();
        } else if (type == List) {
            if (value == nullptr or o.value == nullptr)
                if (value == o.value)
                    return true;
                else
                    return false;
            else
                return list() == o.list();
        } else if (type == Vector) {
            return vector() == o.vector();
        } else if (type == Function) {
            return function() == o.function();
        } else if (type == Procedure) {
            return procedure() == o.procedure();
        } else if (type == Environment) {
            return environment() == o.environment();
        } else if (type == Promise) {
            return promise().get() == o.promise().get();
        } else /*if (type == Port)*/ {
            return false;
        }
    } else return false;
}

bool LispValue::operator != (const LispValue& other) const {
    return !(*this == other);
}

const char* typeName(decltype(LispValue::type) type) {
    switch (type) {
        case LispValue::Atom:
            return "Atom";
        case LispValue::String:
            return "String";
        case LispValue::Number:
            return "Number";
        case LispValue::Bool:
            return "Bool";
        case LispValue::Character:
            return "Character";
        case LispValue::List:
            return "List";
        case LispValue::Vector:
            return "Vector";
        case LispValue::Function:
            return "Function";
        case LispValue::Procedure:
            return "Procedure";
        case LispValue::Environment:
            return "Environment";
        case LispValue::Port:
            return "Port";
        case LispValue::Promise:
            return "Promise";
        default:
            return "Undefined";
    }
}

std::ostream& operator << (std::ostream& out, const LispValue& val) {
    if (val.syntax()) {
        std::cout << "#<syntax ";
    }
    if (val.type == LispValue::Atom) {
        out << val.atom();
    } else if (val.type == LispValue::String) {
        out << '"' << val.string() << '"';
    } else if (val.type == LispValue::Number) {
        out << val.number();
    } else if (val.type == LispValue::Bool) {
        out << (val.boolean() ? "#t" : "#f");
    } else if (val.type == LispValue::Character) {
        out << "#\\";
        if (val.character() == ' ') {
            out << "space";
        } else if (val.character() == '\n') {
            out << "newline";
        } else {
            out << val.character();
        }
    } else if (val.type == LispValue::List) {
        out << '(';
        if (!val.isNull()) {
            auto it = val.list().begin();
            for (; std::next(it).get().type == LispValue::List
                 and !std::next(it).get().isNull(); it++) {
                out << *it << ' ';
            }
            out << *it;
            if (std::next(it).get().type != LispValue::List) {
                out << " . " << std::next(it).get();
            }
        }
        out << ')';
    } else if (val.type == LispValue::Vector) {
        out << "#(";
        for (auto it = val.vector().begin(); it != val.vector().end(); it++) {
            out << *it;
            if (std::next(it) != val.vector().end()) out << ' ';
        }
        out << ')';
    } else if (val.type == LispValue::Function) {
        out << "#<procedure (";
        auto min = val.function().minArgs;
        for (count_t i = 0; i < min; i++) {
            out << '_';
            if (i + 1 != min) out << ' ';
        }
        if (min != val.function().maxArgs) {
            out << " . _";
        }
        out << ")>";
    } else if (val.type == LispValue::Procedure) {
        out << "#<procedure (";
        for (auto it = val.procedure().args.begin();
             it != val.procedure().args.end(); it++) {
            out << *it;
            if (std::next(it) != val.vector().end()) out << ' ';
        }
        if (!val.procedure().valarg.isNull())
            out << " . " << val.procedure().valarg;
        out << ")>";// << val.procedure().body << ">";
    } else if (val.type == LispValue::Environment) {
        out << "#<environment>";
    } else if (val.type == LispValue::Port) {
        out << "#<";
        if (val.port().input) {
            out << "input ";
        } else {
            out << "output ";
        }
        out << "port>";
    } else if (val.type == LispValue::Promise) {
        out << "#<promise " << val.promise().get() << ">";
    } else {
        out << "#<undefined>";
    }
    if (val.syntax()) {
        std::cout << ">";
    }
    return out;
}

LispValue apply(const LispValue& proc, const LispValue& args, const LispValue& env) {
    if (proc.type == LispValue::Procedure or proc.type == LispValue::Function) {
        LispVector argsVec;
        if (!args.isNull()) {
            if (args.type != LispValue::List) {
                throw badSyntax(args, true);
            }
            argsVec.reserve(3);
            auto it = args.list().begin();
            while (true) {
                if (it.get().isNull()) {
                    break;
                } else if (it.get().type != LispValue::List) {
                    throw badSyntax(args, true);
                }
                argsVec.push_back(it->eval(env));
                it++;
            }
        }
//        argsVec.shrink_to_fit();
        if (proc.type == LispValue::Procedure) {
            return proc.procedure().apply(argsVec);
        } else {
            return proc.function().apply(argsVec);
        }
    } else throw badTypeFor("apply", proc);
}

/*!
* \brief Вычисляет значение выражения
* \param env окружение в котором происходит вычисление
* \return Возвращает результат вычисления выражения
*/
LispValue LispValue::eval(const LispValue& env) const {
    // Случай когда выражение является списком
    if (type == List) {
        if (isNull()) return *this;
        bool p;
        LispValue res;
        std::tie(res, p) = runSpecialForm(*this, env);
        if (p) return res;
        auto proc = list().head.eval(env);
        try {
            return apply(proc, list().tail, env);
        } catch (LangException& e) {
            if (e.getAddObj()) {
                e << " " << printErrorObject(*this);
            }
            throw e;
        }

    // Случай когда выражение является атомом
    } else if (type == Atom) {
        if (isSpecialForm(atom())) {
            throw badSpecialForm(*this);
        }
        return env.environment().find(atom());
    } else return *this;
}
