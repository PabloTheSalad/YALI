#include <iostream>
#include <string>
#include <limits>

#include "repl.hpp"
#include "lispvalue.hpp"
#include "types/pairlist.hpp"
#include "types/procedure.hpp"
#include "types/function.hpp"
#include "types/environment.hpp"
#include "parser.hpp"
#include "exception.hpp"
#include "macro.hpp"
#include "config.hpp"

#include <fstream>
#include <iomanip>
#include <chrono>
#include <strstream>

using SClock = std::chrono::high_resolution_clock;
using TimeSlice = std::chrono::microseconds;
using TimePoint = SClock::time_point;

std::pair<TimeSlice, LispValue> time(const LispValue& v, const LispValue& env) {
    TimePoint start = SClock::now();
    auto result = v.eval(env);
    return std::make_pair(std::chrono::duration_cast<TimeSlice>(SClock::now() - start), result);
}

std::tuple<TimeSlice, TimeSlice, TimeSlice, TimeSlice, LispValue>
timeit(const LispValue& v, const LispValue& env, count_t count) {
    TimeSlice allTime(0), minTime(TimeSlice::max()), maxTime(0), cycleTime(0);
    TimePoint start;
    LispValue result;
    for (count_t i = 0; i < count; i++) {
        start = SClock::now();
        result = v.eval(env);
        cycleTime = std::chrono::duration_cast<TimeSlice>(SClock::now() - start);
        if (cycleTime < minTime) minTime = cycleTime;
        if (cycleTime > maxTime) maxTime = cycleTime;
        allTime += cycleTime;
    }
    return std::make_tuple(allTime, minTime, allTime/count, maxTime, result);
}

inline void computeError(const LangException& e, std::istream& in) {
    std::cerr << e.what() << std::endl;
//    in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.putback('\n');
}

void showType(const LispValue& e) {
    auto& out = std::cout;
    if (Config::showType()) {
        out << " : " << typeName(e.type);
        if (e.type == LispValue::Number) {
            out << " (";
            if (e.number().isInteger())
                out << "Integer";
            else if (e.number().isRational())
                out << "Rational";
            else if (e.number().isReal())
                out << "Real";
            out << ")";
        }
    }
}

void printHelp() {
    std::vector<std::pair<std::string, std::string>> help = {
        {"help", "show this help"},
        {"env", "show current environment"},
        {"envmin", "showcurrent environment (for short)"},
        {"time <expr>", "eval <expr> and measure time"},
        {"timeit <expr>", "eval <expr> 100 times and measure time"},
        {"set <param> <on|off>", "set parameter <param> on [on|off]"},
        {"load <file>", "load file <file> in interpreter"},
        {"quit", "close interpreter"}
    };
    std::cout << "Available commands" << std::endl;    for (auto h : help) {
        std::cout << std::setw(32) << std::left << h.first
                   << "- " << h.second << std::endl;
    }
}

void runCommand(std::string& s, std::istream& in, const LispValue& env) {
    if (s == "help") {
        printHelp();
    } else if (s == "env") {
        for (auto pair : env.environment().getValues()) {
            std::cout << std::setw(12) << pair.first << " : " << pair.second.first << std::endl;
        }
    } else if (s == "envmin") {
        for (auto pair : env.environment().getValues()) {
            std::cout << "[ " << pair.first;
            showType(pair.second.first);
            if (pair.second.second) {
                std::cout << " (Macro)";
            }
            std::cout << " ]" << std::endl;
        }
    } else if (s == "time") {
        try {
            auto expr = macroExpansion(parseExpr(in), env);
            TimeSlice vtime;
            std::tie(vtime, expr) = time(expr, env);
            std::cout << "Time: " << vtime.count() << " mcs" << std::endl
                      << "Result: " << expr << std::endl;
        } catch(LangException& e) {
            computeError(e, in);
        }
    } else if (s == "timeit") {
        try {
            auto expr = macroExpansion(parseExpr(in), env);
            TimeSlice allTime, minTime, avTime, maxTime;
            std::tie(allTime, minTime, avTime, maxTime, expr) = timeit(expr, env, 100);
            std::cout << "For 100 cycles: " << std::endl
                      << "All time: "     << allTime.count() << " mcs" << std::endl
                      << "Min time: "     << minTime.count() << " mcs" << std::endl
                      << "Average time: " << avTime.count() << " mcs" << std::endl
                      << "Max time: "     << maxTime.count() << " mcs" << std::endl
                      << "Result: "       << expr << std::endl;
        } catch(LangException& e) {
            computeError(e, in);
        }
    } else if (s == "set") {
        std::string toggle;
        in >> s;
        in >> toggle;
        if (toggle == "on" or toggle == "off") {
            bool p = false;
            if (toggle == "on") p = true;

            if (s == "show-macro") {
                Config::showMacro() = p;
            } else if (s == "show-type") {
               Config::showType() = p;
            }
        }
    } else if (s == "load") {
        std::string filename;
        in >> filename;
        loadFile(filename, env);
    }
    while (isspace(in.peek()) and in.peek() != '\n') in.get();
    in.get();
}

//Parse-Eval Expression
LispValue PEExp(std::istream& in, const LispValue& env) {
    LispValue code = parseExpr(in);
    bool p = false;
    if (in.peek() == '\n') {
        in.get();
        p = true;
    }
    code = macroExpansion(code, env);
    LispValue e;
    if (!(code.type == LispValue::Atom and code.atom() == ""))
        e = code.eval(env);
    if (p) in.putback('\n');
    return e;
}

void loadFile(std::string filename, const LispValue& env, bool v) {
    std::ifstream file(filename);
    if (file.is_open()) {
        while (!file.eof()) {
            skipSpaces(file);
            if (file.peek() == -1) break;
            PEExp(file, env);
        }
        if (v) std::cout << "Load: file loaded: " << filename << std::endl;
    } else {
        if (v) std::cout << "Load: file not found: " << filename << std::endl;
        return;
    }
}

//Parse-Eval-Print Expression
void PEPExp(std::istream& in, const LispValue& env) {
    auto& out = std::cout;
    try {
        auto e = PEExp(in, env);
        if (e.type == LispValue::Undef) return;
        env.environment().define("$it", e);
        out << "res> " << e;
        showType(e);
        out << std::endl;
    } catch (LangException& e) {
        computeError(e, in);
    }
}

void repl(std::istream& in, const LispValue& env) {
    auto& out = std::cout;
    std::cout << "Welcome to YALI - Lisp interpreter\nFor help enter ;help" << std::endl;
    while (true) {
        out << "lisp=> ";
        if (in.peek() == -1) {
            out << std::endl << "Leaving YALI" << std::endl;
            break;
        }
        while (isspace(in.peek()) and in.peek() != '\n') in.get();
        if (in.peek() == '\n') {
            in.get();
            continue;
        }
        if (in.peek() == ';') {
            in.get();
            std::string command;
            in >> command;
            if (command == "quit") {
                out << "Leaving YALI" << std::endl;
                break;
            } else runCommand(command, in, env);
        } else {
            while (true) {
                while (isspace(in.peek()) and in.peek() != '\n') in.get();
                if (in.peek() == '\n') break;
                else if (in.peek() == ';') break;
                PEPExp(in, env);
            }
            in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}
