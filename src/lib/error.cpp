#include "error.hpp"
#include "../lispvalue.hpp"
#include "../exception.hpp"
#include "../types/environment.hpp"
#include "lib.hpp"

LispValue error(LispVector& args) {
    assertArgType("error", LispValue::String, args[0], 1);
    throw userError(args[0].string(), args[1]);
}

LispMap Base::Error::getMap() {
    return {
        {"error", Function(error, 2, 2)},
    };
}
