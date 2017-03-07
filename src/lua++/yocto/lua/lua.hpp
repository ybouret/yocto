#ifndef YOCTO_LUA_INCLUDED
#define YOCTO_LUA_INCLUDED 1


// lua.hpp
// Lua header files for C++
// <<extern "C">> not supplied automatically because Lua also compiles as C++

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#endif
