#ifndef YOCTO_CHEMICAL_LUA_INCLUDED
#define YOCTO_CHEMICAL_LUA_INCLUDED 1

#include "yocto/chemical/equilibria.hpp"
#include "yocto/lua/lua-state.hpp"

namespace yocto
{
    namespace chemical
    {

        struct __lua
        {
            static void load( Lua::State::Pointer &vm, library    &lib );
            static void load( Lua::State::Pointer &vm, equilibria &eqs, library &lib );
        };

    }
}

#endif

