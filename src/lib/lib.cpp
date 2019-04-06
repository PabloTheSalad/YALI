#include "../types/environment.hpp"

#include "lib.hpp"
#include "arithmetic.hpp"
#include "list.hpp"
#include "predicate.hpp"
#include "io.hpp"
#include "vector.hpp"
#include "error.hpp"

static LispValue GLENV;

LispValue& getBaseEnv() {
    if (GLENV.isNull()) {
        LispValue env = Environment();
        env.environment().add_values(Base::Ariphmetic::getMap());
        env.environment().add_values(Base::List::getMap());
        env.environment().add_values(Base::Predicate::getMap());
        env.environment().add_values(Base::Promise::getMap());
        env.environment().add_values(Base::Vector::getMap());
        env.environment().add_values(Base::IO::getMap());
        env.environment().add_values(Base::Error::getMap());
        GLENV = env;
    }
    return GLENV;
}
