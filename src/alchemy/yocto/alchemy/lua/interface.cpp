#include "yocto/alchemy/lua/interface.hpp"

namespace yocto
{
    namespace alchemy
    {
        void __lua:: load( Lua::State &vm, const string &libName, library &lib, species_callback *cb)
        {
            const char *name = libName.c_str();
            lua_State  *L = *vm;
            lua_settop(L,0);
            lua_getglobal(L,name);
            if( !lua_istable(L,-1) )
            {
                throw exception("'%s' is not a lua table", name);
            }
            const size_t n = vm.GetTableLength();
            std::cerr << "Parsing " << n << " species from " << name << std::endl;
        }
    }

}

