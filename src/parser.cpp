#include "parser.hpp"
#include "exception.hpp"
#include "types/pairlist.hpp"
#include "types/number.hpp"
#include <cctype>

LispValue parseValue(std::istream& in);

bool isDelimiter(char c) {
    return isspace(c) or c == '(' or c == ')' or c == ';';
}

void skipSpaces(std::istream& in) {
    while (isspace(in.peek()))
        in.get();
}

LispValue parseExpr(std::istream& in) {
    skipSpaces(in);
    if (in.peek() == ';') { //Комментарий
        while (in.get() != '\n');
    }
    skipSpaces(in);
    if (in.peek() == '(') { //Начало списка
        in.get();

        skipSpaces(in);
        if (in.peek() == ')') { //Пустой список
            in.get();
            return {};
        }

        LispList list{parseExpr(in)};
        if (list.front().type == LispValue::Atom and list.front().atom() == ".")
            throw parseError(")", "unexpected .");
        skipSpaces(in);
        while(in.peek() != ')') {
            auto expr = parseExpr(in);
            if (expr.type == LispValue::Atom and expr.atom() == ".") {
                skipSpaces(in);
                if (in.peek() == ')') throw parseError(")", "unexpected )");
                else {
                   auto tail = parseExpr(in);
                   skipSpaces(in);
                   if (in.peek() != ')') throw parseError("!", "bad pair form");
                   in.get();
                   list.set_end(tail);
                   return std::move(list);
                }
            }
            skipSpaces(in);
            list.push_back(expr);
        }
        in.get();
        return std::move(list);
    } else if (in.peek() == ')')
        throw parseError(")", "unexpected )");
      else return parseValue(in);
}

LispValue parseCharacter(std::istream& in) {
    std::string buffer = "";
    while (!isDelimiter(in.peek())) buffer.push_back(in.get());
    if (buffer == "space") return LispCharacter(' ');
    else if (buffer == "newline") return LispCharacter('\n');
    else if (buffer.size() == 1) return LispCharacter(buffer.front());
    else throw parseError("#\\" + buffer, "bad character name");
}

LispValue parseVector(std::istream& in) {
    LispVector vec;
    skipSpaces(in);
    while(in.peek() != ')') {
        skipSpaces(in);
        vec.push_back(parseExpr(in));
    }
    in.get();
    return std::move(vec);
}

bool ishex(char c) {
    return isdigit(c)
            or (c >= 'A' and c <= 'F')
            or (c >= 'a' and c <= 'f');
}

LispValue parsePrefixNumber(std::istream& in, char prefix) {
    int64_t num = 0;
    std::string buffer;
    while (!isDelimiter(in.peek())) {
        if (!ishex(in.peek()) )
            throw parseError(std::string(in.peek(), 1), "bad number form");
        buffer.push_back(in.get());
    }
    if (prefix == 'b') {
        ///!
    } else if (prefix == 'o') {
        if (sscanf(buffer.data(), "%o", &num) != 1)
            throw parseError(buffer, "bad octal number form");
    } else if (prefix == 'd') {
        if (sscanf(buffer.data(), "%d", &num) != 1)
            throw parseError(buffer, "bad decimal number form");
    } else if (prefix == 'x') {
        if (sscanf(buffer.data(), "%x", &num) != 1)
            throw parseError(buffer, "bad hexdecimal number form");
    }
    return LispNumber(num);
}

LispValue parseNumber(std::istream& in) {
    enum {
        Number,
        Atom
    } state = Number;
    std::string buffer;
    bool dot = false;
    size_t slash = 0;
    auto sign = 1;
    if (in.peek() == '-') {
        sign = -1;
        in.get();
    }
    while(!isDelimiter(in.peek())) {
        if (state == Number) {
            if (in.peek() == '.') {
                if (!dot and slash == 0) {
                    dot = true;
                    buffer.push_back(in.get());
                } else state = Atom;
            } else if (in.peek() == '/') {
                if (slash == 0 and !dot) {
                    slash = buffer.size();
                    if (slash == 0) state = Atom;
                    buffer.push_back(in.get());
                } else state = Atom;
            } else if (!isdigit(in.peek())) state = Atom;
            else buffer.push_back(in.get());
        } else {
            buffer.push_back(in.get());
        }
    }
    if (slash == buffer.size())
        state = Atom;
    if (state == Number) {
        if (dot)
            return Number::fromReal(sign * atof(buffer.data()));
        else if (slash != 0) {
            std::string fn{buffer.begin(), buffer.begin()+slash};
            std::string sn{buffer.begin()+slash+1, buffer.end()};
            auto rat = RatNum(atol(fn.data()), atol(sn.data()));
            if (rat.getDenominator() == 0) {
                return LispValue(std::move(buffer), LispValue::Atom);
            }
            rat.gcd();
            if (rat.getDenominator() == 1)
                return Number::fromInteger(sign * rat.getNumerator());
            else
                return Number::fromRational(RatNum(sign, 1) * rat);
        } else
            return Number::fromInteger(sign * atol(buffer.data()));
    } else {
        if (sign == 1) {
            return LispValue(std::move(buffer), LispValue::Atom);
        } else {
            return LispValue(std::move("-"+buffer), LispValue::Atom);
        }

    }
}

LispValue parseString(std::istream& in) {
    std::string buffer;
    while (in.peek() != '"') buffer.push_back(in.get());
    in.get();
    return LispValue(std::move(buffer), LispValue::String);
}

LispValue parseValue(std::istream& in) {
    if (in.peek() == '#') {
        in.get();
        char ch = in.get();
        if (ch == '\\') return parseCharacter(in);
        else if (ch =='(') return parseVector(in);
        else if (ch== 'b'
                 or ch == 'o'
                 or ch == 'd'
                 or ch == 'x') return parsePrefixNumber(in, ch);
        else if (ch == 't') return true;
        else if (ch == 'f') return false;
        else throw parseError(std::string(ch, 1), "unknown # object");
    } else if (isdigit(in.peek()) or in.peek() == '-') {
        return parseNumber(in);
    }
    else if (in.peek() == '"') {
        in.get();
        return parseString(in);
    } else {
        std::string buffer;
        while (!isDelimiter(in.peek()) and in.peek() != -1) {
            buffer.push_back(in.get());
            if (buffer == "'") {
                return LispList{{"quote", LispValue::Atom}, parseExpr(in)};
            }
        }
        return LispValue(std::move(buffer), LispValue::Atom);
    }
}


