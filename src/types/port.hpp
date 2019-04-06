#ifndef PORT_HPP
#define PORT_HPP

#include <iostream>
#include <unordered_set>

using LispString = std::string;

struct LispPort {
    std::iostream* stream;
    bool input;
    LispPort() : stream(nullptr), input(false) {}
    LispPort(std::iostream* s, bool i) : stream(s), input(i) {}
    ~LispPort() {
        delete stream;
    }
    std::iostream& getStream() const { return *stream; }
    static LispPort* openFile(const LispString&, bool in);
};

class FilePool : public std::unordered_set<LispPort*> {
public:
    ~FilePool() {
        for (auto file : *this) {
            delete file;
        }
    }
    void erase_ptr(LispPort* file) {
        auto res = find(file);
        if (res != end()) {
            delete file;
            std::unordered_set<LispPort*>::erase(res);
        }
    }
};

static FilePool filePool;

#endif // PORT_HPP
