#include "io.hpp"
#include <iostream>
#include "exception.hpp"
#include "parser.hpp"
#include "lib.hpp"
#include <fstream>

auto& lispOut = std::cout;
auto& lispInput = std::cin;

LispValue display(LispVector& args) {
    if (args[0].type == LispValue::String) {
        lispOut << args[0].string();
    } else if (args[0].type == LispValue::Character) {
        lispOut << args[0].character();
    } else
        lispOut << args[0];
    return undefined();
}

LispValue newLine(LispVector& args) {
    if (args.size() == 0) {
        lispOut << std::endl;
    } else {
        assertArgType("newline", LispValue::Port, args[0], 1);
        (std::ofstream&)args[0].port().getStream() << std::endl;
    }
    return undefined();
}

LispValue read(LispVector& args) {
    if (args.size() == 0) {
        return parseExpr(lispInput);
    } else {
        assertArgType("read", LispValue::Port, args[0], 1);
        return parseExpr(args[0].port().getStream());
    }
}

LispValue write(LispVector& args) {
    if (args.size() == 1) {
        lispOut << args[0];
    } else {
        assertArgType("write", LispValue::Port, args[1], 2);
        (std::ofstream&)args[1].port().getStream() << args[0];
    }
    return undefined();
}

LispValue readChar(LispVector& args) {
    if (args.size() == 0) {
        return (char)lispInput.get();
    } else {
        assertArgType("read-char", LispValue::Port, args[0], 1);
        return (char)args[0].port().getStream().get();
    }
}

LispValue writeChar(LispVector& args) {
    assertArgType("write-char", LispValue::Character, args[0], 1);
    if (args.size() == 1) {
        lispOut << args[0].character();
    } else {
        assertArgType("write-char", LispValue::Port, args[1], 2);
        (std::ofstream&)args[1].port().getStream() << args[0].character();
    }
    return undefined();
}

LispValue openInputFile(LispVector& args) {
    assertArgType("open-input-file", LispValue::String, args[0], 1);
    auto file = LispPort::openFile(args[0].string(), true);
    return file;
}

LispValue openOutputFile(LispVector& args) {
    assertArgType("open-output-file", LispValue::String, args[0], 1);
    auto file = LispPort::openFile(args[0].string(), false);
    return file;
}

LispValue closeInputPort(LispVector& args) {
    assertArgType("close-input-port", LispValue::Port, args[0], 1);
    if (!args[0].port().input)
        throw badArgTypeName("close-input-port", args[0], 1, "Input port");
    ((std::ifstream&)args[0].port().getStream()).close();
    return undefined();
}

LispValue closeOutputPort(LispVector& args) {
    assertArgType("close-output-port", LispValue::Port, args[0], 1);
    if (args[0].port().input)
        throw badArgTypeName("close-output-port", args[0], 1, "Output port");
    ((std::ofstream&)args[0].port().getStream()).close();
    return undefined();
}

LispValue isInputPort(LispVector& args) {
    assertArgType("input-port?", LispValue::Port, args[0], 1);
    return LispBool(args[0].port().input);
}

LispValue isOutputPort(LispVector& args) {
    assertArgType("output-port?", LispValue::Port, args[0], 1);
    return LispBool(!args[0].port().input);
}


LispMap Base::IO::getMap() {
    return {
        {"display", Function(display, 1, 1)},
        {"newline", Function(newLine, 0, MAX_COUNT)},
        {"read", Function(read, 0, 1)},
        {"write", Function(write, 1, 2)},
        {"read-char", Function(readChar, 0, 1)},
        {"write-char", Function(writeChar, 1, 2)},
        {"open-input-file", Function(openInputFile, 1, 1)},
        {"open-output-file", Function(openOutputFile, 1, 1)},
        {"close-input-port", Function(closeInputPort, 1, 1)},
        {"close-output-port", Function(closeOutputPort, 1, 1)},
        {"input-port?", Function(isInputPort, 1, 1)},
        {"output-port?", Function(isOutputPort, 1, 1)}
    };
}
