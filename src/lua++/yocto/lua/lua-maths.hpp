#ifndef YOCTO_LUA_MATHS_INCLUDED
#define YOCTO_LUA_MATHS_INCLUDED 1

#include "yocto/lua/lua-state.hpp"
#include "yocto/math/types.hpp"
#include "yocto/exception.hpp"

namespace yocto
{
    namespace Lua
    {
        class _Function
        {
        public:
            _Function(const State::Pointer &state, const string &fname);
            _Function(const _Function &fn);

            virtual ~_Function() throw();

            State::Pointer vm;
            const string   fn;

        protected:
            lua_State *fetch();

        private:
            YOCTO_DISABLE_ASSIGN(_Function);
        };

        template <typename T>
        class Function : public _Function
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;
            inline Function(const State::Pointer &state, const string &fname) :
            _Function(state,fname) {}
            inline Function(const Function &other) : _Function(other) {}
            inline virtual ~Function() throw() {}


            //! one parameter function
            inline T operator()(param_type arg)
            {
                lua_State *L = fetch();
                vm->Push<T>(arg);
                if(lua_pcall(L, 1, 1, 0))
                {
                    throw exception("%s error:'%s'",fn.c_str(),lua_tostring(L, -1));
                }
                return vm->To<T>(-1);
            }

            //! two parameters function
            inline T operator()(param_type arg1, param_type arg2)
            {
                lua_State *L = fetch();
                vm->Push<T>(arg1);
                vm->Push<T>(arg2);
                if(lua_pcall(L, 2, 1, 0))
                {
                    throw exception("%s error:'%s'",fn.c_str(),lua_tostring(L, -1));
                }
                return vm->To<T>(-1);
            }



        private:
            YOCTO_DISABLE_ASSIGN(Function);
        };
    }

}

#endif
