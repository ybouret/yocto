#ifndef YOCTO_CHEMICAL_LUA_INCLUDED
#define YOCTO_CHEMICAL_LUA_INCLUDED 1

#include "yocto/chemical/equilibria.hpp"
#include "yocto/lua/lua.hpp"

namespace yocto
{
    namespace chemical
    {

        struct __lua
        {
            static void load( lua_State *L, library &lib );
        };

    }
}

#endif

