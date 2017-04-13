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
            //__________________________________________________________________
            //
            // Library API
            //__________________________________________________________________
            typedef functor<void,TL2(Lua::State&,species&)> species_callback;

            static void load(Lua::State::Pointer   &vm,
                             const string          &libName,
                             library               &lib,
                             species_callback      *cb = 0);


            //__________________________________________________________________
            //
            // Equilibria API
            //__________________________________________________________________
            static void load(Lua::State::Pointer &vm,
                             const string        &eqsName,
                             equilibria          &eqs
                             );

            //__________________________________________________________________
            //
            // Boot API
            //__________________________________________________________________
            static void load(Lua::State::Pointer &vm,
                             const string        &bootName,
                             boot                &loader);

            //__________________________________________________________________
            //
            // Solution API
            //__________________________________________________________________
            //! push array as a solution
            static void push( lua_State *L, const array<double> &S, const library &lib);


            //! read solution on stack
            static void load( lua_State *L, array<double> &S, const library &lib);
        };

    }
}

#endif
