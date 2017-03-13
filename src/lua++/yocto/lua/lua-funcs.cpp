#include "yocto/lua/lua-funcs.hpp"

namespace yocto
{
    namespace Lua
    {
        _Function:: _Function( const _Function &other ) :
        vm(other.vm),
        fn(other.fn)
        {
        }

        _Function:: _Function(const State::Pointer &state, const string &fname) :
        vm(state),
        fn(fname)
        {
        }

        _Function:: ~_Function() throw()
        {
        }

        lua_State *_Function:: fetch()
        {
            lua_State *L = **vm;
            //lua_settop(L,0);
            lua_getglobal(L,fn.c_str());
            return L;
        }

    }

}
