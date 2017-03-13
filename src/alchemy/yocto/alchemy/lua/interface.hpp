#ifndef YOCTO_ALCHEMY_LUA_INTERFACE_INCLUDED
#define YOCTO_ALCHEMY_LUA_INTERFACE_INCLUDED 1

#include "yocto/lua/lua-funcs.hpp"
#include "yocto/alchemy/boot.hpp"

namespace yocto
{
    namespace alchemy
    {

        struct __lua
        {
            // loading species
            typedef functor<void,TL2(Lua::State&,species&)> species_callback;

            static void load(Lua::State::Pointer   &vm,
                             const string          &libName,
                             library               &lib,
                             species_callback      *cb = 0);


            // loading equilibria
            static void load(Lua::State::Pointer &vm,
                             const string        &eqsName,
                             equilibria          &eqs
                             );

            // loading constraints
            static void load(Lua::State::Pointer &vm,
                             const string        &bootName,
                             boot                &loader);

        };

    }
}

#endif
