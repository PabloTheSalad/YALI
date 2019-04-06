#include "function.hpp"
#include "exception.hpp"

LispValue Function::apply(LispVector& args) const {
    if (args.size() < minArgs or args.size() > maxArgs) {
        throw badArgsValue(*this, minArgs, args.size());
    } else return func(args);
}
