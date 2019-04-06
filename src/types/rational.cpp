#include "rational.hpp"
#include <numeric>
#include <float.h>
#include <cmath>

RatNum RatNum::fromReal(const long double& n, const long double& eps) {
    RatNum num(1, 1);
    num.numerator = n/eps;
    num.denominator = 1/eps;
    num.gcd();
    return num;
}

void RatNum::gcd() {
    int32_t div = std::abs(numerator),
              div2 = denominator;
    while ( div2 != 0) { //GCD
        auto r = div % div2;
        div = div2;
        div2 = r;
    }
    numerator /= div;
    denominator /= div;
}

RatNum& RatNum::operator += (const RatNum& n) {
    numerator = numerator*(int32_t)n.denominator+n.numerator*(int32_t)denominator;
    denominator = denominator*n.denominator;
    gcd();
    return *this;
}

RatNum RatNum::operator + (const RatNum& n) const {
    RatNum new_num(*this);
    new_num += n;
    return new_num;
}

RatNum& RatNum::operator -= (const RatNum& n) {
    numerator = numerator*(int32_t)n.denominator-n.numerator*(int32_t)denominator;
    denominator = denominator*n.denominator;
    gcd();
    return *this;
}

RatNum RatNum::operator - (const RatNum& n) const {
    RatNum new_num(*this);
    new_num -= n;
    return new_num;
}

RatNum RatNum::operator - () const {
    RatNum new_num(*this);
    new_num.numerator = -new_num.numerator;
    return new_num;
}

RatNum& RatNum::operator *= (const RatNum& n) {
    numerator = numerator*n.numerator;
    denominator = denominator*n.denominator;
    gcd();
    return *this;
}

RatNum RatNum::operator * (const RatNum& n) const {
    RatNum new_num(*this);
    new_num *= n;
    return new_num;
}

RatNum& RatNum::operator /= (const RatNum& n) {
    numerator *= (int32_t)n.denominator;
    denominator *= (u_int32_t)n.numerator;
    gcd();
    return *this;
}

RatNum RatNum::operator / (const RatNum& n) const {
    RatNum new_num(*this);
    new_num /= n;
    return new_num;
}

bool RatNum::operator < (const RatNum& n) const {
    return numerator*n.denominator < n.numerator*denominator;
}

bool RatNum::operator > (const RatNum& n) const {
    return numerator*n.denominator > n.numerator*denominator;
}

bool RatNum::operator <= (const RatNum& n) const {
    return *this < n or *this == n;
}

bool RatNum::operator >= (const RatNum& n) const {
    return *this > n or *this == n;
}

std::ostream& operator << (std::ostream& out, const RatNum& n) {
    out << n.numerator << "/" << n.denominator;
    return out;
}

RatNum RatNum::abs(const RatNum& n) {
    RatNum new_num(n);
    new_num.numerator = std::abs(new_num.numerator);
    return new_num;
}

RatNum RatNum::pow(const RatNum& b, const int32_t& e) {
    RatNum n(std::pow(b.numerator, e), std::pow(b.denominator, e));
    n.gcd();
    return n;
}
