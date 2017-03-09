#ifndef YOCTO_LUA_MATHS_INCLUDED
#define YOCTO_LUA_MATHS_INCLUDED 1

#include "yocto/lua/lua-state.hpp"
#include "yocto/math/types.hpp"

namespace yocto
{
    namespace Lua
    {
        class Function
        {
        public:
            Function(const State::Pointer &state, const string &fname);
            Function(const Function &fn);

            virtual ~Function() throw();

            State::Pointer vm;
            const string   name;

        private:
            YOCTO_DISABLE_ASSIGN(Function);
        };
    }

}

#endif
