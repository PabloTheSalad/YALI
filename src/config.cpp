#include "config.hpp"

static bool macroShow = false;
static bool typeShow = true;

bool& Config::showMacro() { return macroShow; }

bool& Config::showType() { return typeShow; }
