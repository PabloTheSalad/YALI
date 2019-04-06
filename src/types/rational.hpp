#ifndef RATIONAL_HPP
#define RATIONAL_HPP

#include <algorithm>
#include <ostream>

using u_int32_t = uint32_t;

class RatNum {
    int32_t numerator;
    u_int32_t denominator;
    friend std::ostream& operator << (std::ostream&, const RatNum& n);
    friend struct std::hash<RatNum>;
public:
    RatNum(const RatNum&) = default;
    RatNum(int32_t n, u_int32_t d) : numerator(n), denominator(d) {}
    static RatNum fromReal(const long double& n, const long double& eps);
    void gcd();
    inline int32_t getNumerator() const { return numerator; }
    inline u_int32_t getDenominator() const { return denominator; }
    RatNum& operator += (const RatNum& n);
    RatNum operator + (const RatNum& n) const;
    RatNum& operator -= (const RatNum& n);
    RatNum operator - (const RatNum& n) const;
    RatNum operator - () const;
    RatNum& operator *= (const RatNum& n);
    RatNum operator * (const RatNum& n) const;
    RatNum& operator /= (const RatNum& n);
    RatNum operator / (const RatNum& n) const;
    bool operator < (const RatNum& n) const;
    bool operator > (const RatNum& n) const;
    bool operator <= (const RatNum& n) const;
    bool operator >= (const RatNum& n) const;
    inline bool operator == (const RatNum& n) const {
        return numerator == n.numerator and denominator == n.denominator;
    }
    inline bool operator != (const RatNum& n) const {
        return !(*this == n);
    }
    inline long double toReal() const {
        return numerator/(long double)denominator;
    }
    static RatNum abs(const RatNum& n);
    static RatNum pow(const RatNum&, const int32_t& e);
};

std::ostream& operator << (std::ostream&, const RatNum& n);

#endif // RATIONAL_HPP
