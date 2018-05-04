#include "yocto/chemical/lua.hpp"


namespace yocto
{
    namespace chemical
    {

        // loading a { "species", z } table
        static inline void __add_species( lua_State *L, library &lib, const int count )
        {
            // get name
            assert( lua_istable(L,-1) );
            lua_rawgeti(L,-1,1);
            if(!lua_isstring(L,-1)) throw exception("in '%s': invalid species#%d name",*lib.name,count);
            const string name = lua_tostring(L,-1);
            lua_pop(L,1);


            // get charge
            assert( lua_istable(L,-1) );
            lua_rawgeti(L,-1,2);
            if(!lua_isnumber(L,-1)) throw exception("in '%s': invalid species#%d charge",*lib.name,count);
            const int  charge = int(lua_tonumber(L,-1));
            lua_pop(L,1);

            (void)lib.add(name,charge);

            const species &sp = lib(name);
            std::cerr << "\tadded " << sp.name << ", z=" << sp.z << std::endl;
        }

        void __lua:: load( Lua::State::Pointer &vm, library &lib )
        {
            std::cerr << "lua: loading library " << lib.name << std::endl;
            lua_State *L = **vm;
            lua_settop(L,0);
            const char *name = *(lib.name);
            lua_getglobal(L,name);
            if(!lua_istable(L,-1))
            {
                throw exception("couldn't find library '%s'", name);
            }

            // Lua loop over table
            lua_pushnil(L);  /* first key */
            int count = 0;
            while (lua_next(L,-2) != 0) {
                /* uses 'key' (at index -2) and 'value' (at index -1) */
                /*
                 printf("%s - %s\n",
                 lua_typename(L, lua_type(L, -2)),
                 lua_typename(L, lua_type(L, -1)));
                 */
                /* removes 'value'; keeps 'key' for next iteration */
                __add_species(L,lib,++count);
                lua_pop(L, 1);
            }

            lib.compile();
        }

    }
}
