#include "yocto/lua/lua-state.hpp"
#include "yocto/exceptions.hpp"
#include <cerrno>
#include <iostream>

namespace yocto
{
    namespace Lua
    {

        //! Lua Custom Memory Allocator => Thread Safe
        static
        void *lua_vm_memory( void *, void *ptr, size_t osize, size_t nsize ) throw()
        {
            (void)osize;
            YOCTO_GIANT_LOCK();
            if (nsize == 0) {
                free(ptr);
                return NULL;
            }
            else
            {
                return realloc(ptr, nsize);
            }
        }


        State:: ~State() throw()
        {
            lua_close(L);
            L = 0;
        }

        State:: State() :
        L( lua_newstate( lua_vm_memory, NULL) )
        {
            if( !L )
                throw libc::exception( ENOMEM, "lua_newstate" );
            luaL_openlibs(L);
        }

        lua_State * State:: operator*()
        {
            assert(L);
            return L;
        }

        void State:: DumpStack() {
            printf("<lua_State top='%d'>\n", lua_gettop(L) );
            for( int i=lua_gettop(L);i>0;--i) {
                printf("\t%d\t<%s>\n", i, luaL_typename(L,i));
            }
            printf("</lua_State>\n");
        }

        void State:: DumpTable( int t, const char *name ) {
            const char *tab = name ? name : "TABLE";
            printf("<%s>\n",tab);
            luaL_checktype( L, t, LUA_TTABLE);
            lua_pushnil(L);  /* first key */
            while (lua_next(L, t) != 0) {
                /* uses 'key' (at index -2) and 'value' (at index -1) */
                printf("\t%s - %s\n",
                       lua_typename(L, lua_type(L, -2)),
                       lua_typename(L, lua_type(L, -1)));
                /* removes 'value'; keeps 'key' for next iteration */
                lua_pop(L, 1);
            }
            printf("</%s>\n", name);
        }

        template <>
        double State::Get<double>(const string &name)
        {
            lua_settop(L,0);
            lua_getglobal(L,name.c_str());
            if(!lua_isnumber(L,-1)) throw exception("lua_tonumber failure for '%s'", name.c_str());
            return double(lua_tonumber(L,-1));
        }

        template <>
        int State::Get<int>(const string &name)
        {
            lua_settop(L,0);
            lua_getglobal(L,name.c_str());
            if(!lua_isinteger(L,-1)) throw exception("lua_tointeger failure for '%s'", name.c_str());
            return int(lua_tointeger(L,-1));
        }


        template <>
        string State::Get<string>(const string &name)
        {
            lua_settop(L,0);
            lua_getglobal(L,name.c_str());
            if(!lua_isstring(L,-1)) throw exception("lua_tostring failure for '%s'", name.c_str());
            size_t      l = 0;
            const char *s = lua_tolstring(L, -1, &l);
            return string(s,l);
        }

        void State:: DoString(const string &code)
        {
            lua_settop(L,0);
            //std::cerr << "DoString('" << code << "')" << std::endl;
            if(luaL_dostring(L,code.c_str()))
            {
                throw exception("luaL_dostring: %s", lua_tostring(L,-1) );
            }
        }

        void State:: DoFile(const string &filename)
        {
            lua_settop(L,0);
            if(luaL_dofile(L,filename.c_str()))
            {
                throw exception("luaL_dofile: %s", lua_tostring(L,-1) );
            }
        }

    }
}
