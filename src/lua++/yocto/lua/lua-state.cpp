#include "yocto/lua/lua-state.hpp"
#include "yocto/exceptions.hpp"
#include <cerrno>

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

        
    }
}
