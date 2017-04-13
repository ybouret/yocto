#ifndef YOCTO_ALCHEMY_LUA_INTERFACE_INCLUDED
#define YOCTO_ALCHEMY_LUA_INTERFACE_INCLUDED 1

#include "yocto/lua/lua-funcs.hpp"
#include "yocto/alchemy/boot.hpp"
#include "yocto/alchemy/effectors.hpp"

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

            //! add the content of libName to lib
            static void load(Lua::State::Pointer   &vm,
                             const string          &libName,
                             library               &lib,
                             species_callback      *cb = 0);


            //! helper class
            class Library : public library
            {
            public:
                inline Library(Lua::State::Pointer   &vm,
                               const string          &libName,
                               species_callback      *cb = 0 ) : library()
                {
                    __lua::load(vm,libName,*this,cb);
                }

                inline virtual ~Library() throw()
                {
                }

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Library);
            };

            //__________________________________________________________________
            //
            // Equilibria API
            //__________________________________________________________________
            static void load(Lua::State::Pointer &vm,
                             const string        &eqsName,
                             equilibria          &eqs
                             );


            class Equilibria : public equilibria
            {
            public:
                inline Equilibria(Lua::State::Pointer    &vm,
                                  const string           &eqsName,
                                  const library::pointer &plib ) : equilibria(plib)
                {
                    __lua::load(vm,eqsName,*this);
                }

                inline virtual ~Equilibria() throw()
                {
                }

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Equilibria);
            };

            //__________________________________________________________________
            //
            // Boot API
            //__________________________________________________________________
            static void load(Lua::State::Pointer &vm,
                             const string        &bootName,
                             boot                &loader);

            class Boot : public boot
            {
            public:
                inline Boot(Lua::State::Pointer    &vm,
                            const string           &bootName,
                            const library::pointer &plib) :
                boot(plib)
                {
                    __lua::load(vm,bootName,*this);
                }

                inline virtual ~Boot() throw() {}

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Boot);
            };


            //__________________________________________________________________
            //
            // Solution API
            //__________________________________________________________________
            //! push array as a solution
            static void push( lua_State *L, const array<double> &S, const library &lib);


            //! read solution on stack
            static void load( lua_State *L, array<double> &S, const library &lib);


            //__________________________________________________________________
            //
            // parameters API
            //__________________________________________________________________
            //! push a table of parameters from the variables
            static void push( lua_State *L, const array<double> &A, const variables &params);

            //! load values from params loading name
            static void load( lua_State *L, array<double> &A, const variables &params);


            //__________________________________________________________________
            //
            // effectors API
            //__________________________________________________________________
            static void load( lua_State *L, effectors &edb, const string &id);

            class Effectors : public effectors
            {
            public:
                inline Effectors(Lua::State::Pointer    &vm,
                                 const string           &edbName) :
                effectors()
                {
                    __lua::load(**vm,*this,edbName);
                }

                inline virtual ~Effectors() throw() {}


            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Effectors);
            };


        };

    }
}

#endif
