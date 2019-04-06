#include <iostream>
#include "lib/lib.hpp"
#include "types/environment.hpp"
#include "repl.hpp"
#include "config.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    auto& env = getBaseEnv();
    for (int i = 0; i < Config::filesCount; i++) {
        loadFile(Config::filesToLoad[i], env, argc == 1);
    }
    if (argc == 1) {
        repl(std::cin, env);
    } else {
        for (int i = 1; i < argc; i++) {
            loadFile(argv[i], env, false);
        }
    }
    return 0;
}
