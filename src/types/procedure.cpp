#include "procedure.hpp"
#include "exception.hpp"
#include "environment.hpp"
#include "pairlist.hpp"

LispValue Procedure::apply(LispVector& values) {
    if ((values.size() != args.size()) and valarg.isNull())
        throw badArgsValue(*this, args.size(), values.size());
    else {
        Environment argEnv(env);
        auto valIt = values.begin();
        for (auto arg : args) {
            argEnv.define(arg.string(), *valIt);
            valIt++;
        }
        if (!valarg.isNull()) {
            if (valIt == values.end()) argEnv.define(valarg.string(), {});
            else {
                LispValue list = PairList{*valIt};
                valIt++;
                for (; valIt != values.end(); valIt++) {
                    list.list().push_back(*valIt);
                }
                argEnv.define(valarg.string(), list);
            }
        }
        return body.eval(std::move(argEnv));
    }
}
