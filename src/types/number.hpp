#ifndef NUMBER_HPP
#define NUMBER_HPP

#include <algorithm>
#include "rational.hpp"

class Number {
    enum {
        Real,
        Rational,
        Integer,
    } mType;
    union {
        int64_t integer;
        RatNum rational;
        long double real;
    };
    friend std::ostream& operator << (std::ostream&, const Number&);
public:
    Number(const Number&) = default;
    Number(const int64_t& n) : mType(Integer), integer(n) {}
    Number(const RatNum& n) : mType(Rational), rational(n) {}
    Number(const int32_t& n, const u_int32_t& d) : mType(Rational), rational(n, d) {}
    Number(const long double& n) : mType(Real), real(n) {}
    Number& operator += (const Number& n);
    Number operator + (const Number& n) const;
    Number& operator -= (const Number& n);
    Number operator - (const Number& n) const;
    Number operator - () const;
    Number& operator *= (const Number& n);
    Number operator * (const Number& n) const;
    Number& operator /= (const Number& n);
    Number operator / (const Number& n) const;
    bool operator < (const Number& n) const;
    bool operator > (const Number& n) const;
    bool operator <= (const Number& n) const;
    bool operator >= (const Number& n) const;
    bool operator == (const Number& n) const;
    bool operator != (const Number& n) const;
    decltype(mType) type() const { return mType; }
    void toReal();
    void toRational();
    void toInteger();
    void tryRaise();
    inline static Number fromReal(const long double& n) { return Number(n); }
    inline static Number fromRational(const RatNum& n) { return Number(n); }
    inline static Number fromInteger(const int64_t& n) { return Number(n); }
    static long double numToReal(const Number&);
    static RatNum numToRational(const Number&);
    static int64_t numToInteger(const Number&);
    inline long double getReal() const { return real; }
    inline RatNum getRational() const { return rational; }
    inline int64_t getInteger() const { return integer; }
    inline bool isReal() const { return mType == Real; }
    inline bool isRational() const { return mType == Rational; }
    inline bool isInteger() const { return mType == Integer; }
    static Number abs(const Number&);
    static Number pow(const Number&, const Number&);
};

namespace std {
template<>
struct hash<Number> {
public:
    size_t operator()(const Number& v) const {
        std::hash<long double> h;
        return h(Number::numToReal(v));
    }
};
}

std::ostream& operator << (std::ostream&, const Number&);

#endif // NUMBER_HPP
