#ifndef YOCTO_ALCHEMY_LUA_INTERFACE_INCLUDED
#define YOCTO_ALCHEMY_LUA_INTERFACE_INCLUDED 1

#include "yocto/lua/lua-funcs.hpp"
#include "yocto/alchemy/equilibria.hpp"

namespace yocto
{
    namespace alchemy
    {

        struct __lua
        {
            typedef functor<void,TL2(Lua::State&,species&)> species_callback;

            static void load(Lua::State   &vm,
                             const string &libName,
                             library      &lib,
                             species_callback *cb = 0);

            static void load(Lua::State   &vm,
                             const string &eqsName,
                             equilibria   &eqs
                             );

        };

    }
}

#endif
