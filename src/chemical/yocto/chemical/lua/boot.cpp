
#include "yocto/chemical/lua.hpp"


namespace yocto
{
    namespace chemical
    {

        static inline
        void __add_constraint(Lua::State::Pointer &vm,
                              boot                &loader,
                              library             &lib,
                              const int            count)
        {
            const char *name = *(loader.name);
            lua_State  *L    = **vm;
            if( !lua_istable(L,-1) )
            {
                throw exception("boot.%s: constraint#%d is not a table",name,count);
            }

            // get the constraint value
            const size_t n=vm->GetTableLength();
            std::cerr << "parsing constraint with #" << n << " items" << std::endl;
            if(n<=0)
            {
                throw exception("boot.%s: empty constraint#%d",name,count);
            }

            lua_rawgeti(L,-1,1);
            switch( lua_type(L,-1) )
            {
                case LUA_TNUMBER:
                    std::cerr << "will be a generic constraint" << std::endl;
                    break;

                case LUA_TSTRING:
                    std::cerr << "will be a precomputed constraint" << std::endl;;
                    break;

                default:
                    throw exception("boot.%s#%d: invalid first item, must be a number or a string",name,count);
            }
            lua_pop(L,1);



        }

        void __lua::load(Lua::State::Pointer &vm,
                         boot                &loader,
                         library             &lib)
        {
            lua_State *L = **vm;
            lua_settop(L,0);
            const char *name = *(loader.name);
            lua_getglobal(L,name);
            if(!lua_istable(L,-1))
            {
                throw exception("couldn't find boot '%s'", name);
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
                __add_constraint(vm,loader,lib,++count);
                lua_pop(L, 1);
            }
            lua_settop(L,0);

        }
        
    }

}


