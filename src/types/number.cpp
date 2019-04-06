#include "number.hpp"
#include <cmath>
#include <exception.hpp>

Number& Number::operator += (const Number& n) {
    auto minType = std::min(mType, n.mType);
    if (minType == Real) {
        toReal();
        real += numToReal(n);
    } else if (minType == Rational) {
        toRational();
        rational += numToRational(n);
        tryRaise();
    } else if (minType == Integer) {
        toInteger();
        integer += numToInteger(n);
    }
    return *this;
}

Number Number::operator + (const Number& n) const {
    Number new_num(*this);
    new_num += n;
    return new_num;
}

Number& Number::operator -= (const Number& n) {
    auto minType = std::min(mType, n.mType);
    if (minType == Real) {
        toReal();
        real -= numToReal(n);
    } else if (minType == Rational) {
        toRational();
        rational -= numToRational(n);
        tryRaise();
    } else if (minType == Integer) {
        toInteger();
        integer -= numToInteger(n);
    }
    return *this;
}

Number Number::operator - (const Number& n) const {
    Number new_num(*this);
    new_num -= n;
    return new_num;
}

Number Number::operator - () const {
    if (mType == Real) {
        return -real;
    } else if (mType == Rational) {
        return -rational;
    } else if (mType == Integer) {
        return -integer;
    }
}

Number& Number::operator *= (const Number& n) {
    auto minType = std::min(mType, n.mType);
    if (minType == Real) {
        toReal();
        real *= numToReal(n);
    } else if (minType == Rational) {
        toRational();
        rational *= numToRational(n);
        tryRaise();
    } else if (minType == Integer) {
        toInteger();
        integer *= numToInteger(n);
    }
    return *this;
}

Number Number::operator * (const Number& n) const {
    Number new_num(*this);
    new_num *= n;
    return new_num;
}

Number& Number::operator /= (const Number& n) {
    auto minType = std::min(mType, n.mType);
    if (minType == Integer)
        minType = Rational;
    if ((minType == Integer || minType == Rational) and n == Number::fromInteger(0)) {
        throw divisionByZeroError("/");
    }
    if (minType == Real) {
        toReal();
        real /= numToReal(n);
    } else if (minType == Rational) {
        toRational();
        rational /= numToRational(n);
        tryRaise();
    } else if (minType == Integer) {
        toInteger();
        integer /= numToInteger(n);
    }
    return *this;
}

Number Number::operator / (const Number& n) const {
    Number new_num(*this);
    new_num /= n;
    return new_num;
}

bool Number::operator < (const Number& n) const {
    auto minType = std::min(mType, n.mType);
    if (minType == Real) {
        return numToReal(*this) < numToReal(n);
    } else if (minType == Rational) {
        return numToRational(*this) < numToRational(n);
    } else if (minType == Integer) {
        return numToInteger(*this) < numToInteger(n);
    }
}

bool Number::operator > (const Number& n) const {
    auto minType = std::min(mType, n.mType);
    if (minType == Real) {
        return numToReal(*this) > numToReal(n);
    } else if (minType == Rational) {
        return numToRational(*this) > numToRational(n);
    } else if (minType == Integer) {
        return numToInteger(*this) > numToInteger(n);
    }
}

bool Number::operator <= (const Number& n) const {
    return *this < n or *this == n;
}

bool Number::operator >= (const Number& n) const {
    return *this > n or *this == n;
}

bool Number::operator == (const Number& n) const {
    auto minType = std::min(mType, n.mType);
    if (minType == Real) {
        return numToReal(*this) == numToReal(n);
    } else if (minType == Rational) {
        return numToRational(*this) == numToRational(n);
    } else if (minType == Integer) {
        return numToInteger(*this) == numToInteger(n);
    }
}

bool Number::operator != (const Number& n) const {
    return !(*this == n);
}

std::ostream& operator << (std::ostream& out, const Number& n) {
    if (n.type() == Number::Real) {
        out << n.real;
    } else if (n.type() == Number::Rational) {
        out << n.rational;
    } else if (n.type() == Number::Integer) {
        out << n.integer;
    }
    return out;
}

void Number::toReal() {
    if (mType == Integer) {
        real = integer;
    } else if (mType == Rational) {
        real = rational.toReal();
    }
    mType = Real;
}

void Number::toRational() {
    if (mType == Integer) {
        rational = RatNum(integer, 1);
    } else if (mType == Real) {
        rational = RatNum::fromReal(real, 0.00001);
    }
    mType = Rational;
}

void Number::toInteger() {
    if (mType == Rational) {
        integer = rational.toReal();
    } else if (mType == Real) {
        integer = real;
    }
    mType = Integer;
}

void Number::tryRaise() {
    if (mType == Rational) {
        if (rational.getDenominator() == 1) {
            mType = Integer;
            integer = rational.getNumerator();
        } else if (rational.getNumerator() == 0) {
            mType = Integer;
            integer = 0;
        }
    }
}

long double Number::numToReal(const Number& n) {
    if (n.type() == Integer) {
        return n.integer;
    } else if (n.type() == Rational) {
        return n.rational.toReal();
    } else {
        return n.real;
    }
}

RatNum Number::numToRational(const Number& n) {
    if (n.type() == Integer) {
        return RatNum(n.integer, 1);
    } else if (n.type() == Real) {
        return RatNum::fromReal(n.real, 0.00001);
    } else {
        return n.rational;
    }
}

int64_t Number::numToInteger(const Number& n) {
    if (n.type() == Rational) {
        return n.rational.toReal();
    } else if (n.type() == Real) {
        return n.real;
    } else {
        return n.integer;
    }
}

Number Number::abs(const Number& n) {
    Number new_num(n);
    if (new_num.type() == Integer) {
        new_num.integer = std::abs(new_num.integer);
    } else if (new_num.type() == Rational) {
        new_num.rational = RatNum::abs(new_num.rational);
    } else if (new_num.type() == Real) {
       new_num.real = std::fabs(new_num.real);
    }
    return new_num;
}

Number Number::pow(const Number& b, const Number& e) {
    auto minType = std::min(b.type(), e.type());
    if (minType == Real) {
        return fromReal(std::pow(numToReal(b), numToReal(e)));
    } else if (minType == Rational) {
        if (e.type() == Rational)
            return fromReal(std::pow(numToReal(b), numToReal(e)));
        else if (e.type() == Integer)
            return fromRational(RatNum::pow(numToRational(b), numToInteger(e)));
    } else if (minType == Integer) {
        return fromInteger(std::pow(numToInteger(b), numToInteger(e)));
    }
}

