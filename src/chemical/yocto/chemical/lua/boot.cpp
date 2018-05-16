
#include "yocto/chemical/lua.hpp"


namespace yocto
{
    namespace chemical
    {

        static inline void __add_generic_constraint(Lua::State::Pointer &vm,
                                                    boot                &loader,
                                                    library             &lib,
                                                    const int            count,
                                                    const size_t         n)
        {
            const char *name = *(loader.name);
            lua_State  *L    = **vm;

            if(n<2)
            {
                throw exception("boot.%s#%d: not enough items!",name,count);
            }

            //__________________________________________________________________
            //
            // create the constraint
            //__________________________________________________________________
            boot::constraint &cc = loader.create(lua_tonumber(L,-1));
            // remove value from the stack
            lua_pop(L,1);

            // parse items
            for(unsigned j=2,jm=1;j<=n;++j,++jm)
            {
                assert(lua_istable(L,-1)); // original constraint
                //______________________________________________________________
                //
                // get item { weight, name }
                //______________________________________________________________
                lua_rawgeti(L,-1, j);
                if(!lua_istable(L,-1))
                {
                    throw exception("boot.%s#%d: item %u must be a table",name,count,jm);
                }
                if(2!=vm->GetTableLength())
                {
                    throw exception("boot.%s#%d: item %u must be a table with weight/name fields",name,count,jm);
                }

                //______________________________________________________________
                //
                // weight
                //______________________________________________________________
                lua_rawgeti(L,-1,1);
                if(!lua_isinteger(L,-1))
                {
                    throw exception("boot.%s#%d: item %u weight (first field) is not an integer",name,count,jm);
                }
                const int w = lua_tointeger(L,-1);
                lua_pop(L,1);

                //______________________________________________________________
                //
                // id
                //______________________________________________________________
                lua_rawgeti(L,-1,2);
                if(!lua_isstring(L,-1))
                {
                    throw exception("boot.%s#%d: item %u name (second field) is not a string",name,count,jm);
                }
                const string id = vm->To<string>(-1);
                lua_pop(L,1);
                std::cerr << "\tname  =" << id  << std::endl;
                lua_pop(L,1);

                //______________________________________________________________
                //
                // add the component
                //______________________________________________________________
                species::pointer *pp = lib.search(id);
                if(!pp)
                {
                    throw exception("boot.%s#%d: item %u: species '%s' doesn't exists!",name,count,jm,*id);
                }
                cc.add(**pp,w);
            }


        }

        static inline void __add_specific_constraint(Lua::State::Pointer &vm,
                                                     boot                &loader,
                                                     library             &lib,
                                                     const int            count,
                                                     const size_t         n)
        {
            const char *name = *(loader.name);
            lua_State  *L    = **vm;

            //__________________________________________________________________
            //
            // remove the string
            //__________________________________________________________________
            const string label = vm->To<string>(-1);
            lua_pop(L,1);

            assert(lua_istable(L,-1));
            
            if( "E/N" == label )
            {
                loader.electroneutrality(lib);
                return;
            }


            throw exception("boot.%s#%d: unknowm constraint '%s'", name,count,*label);

        }

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
                case LUA_TNUMBER:  __add_generic_constraint(vm,loader,lib,count,n);  break;
                case LUA_TSTRING:  __add_specific_constraint(vm,loader,lib,count,n); break;
                default: throw exception("boot.%s#%d: invalid first item, must be a number or a string",name,count);
            }

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
                __add_constraint(vm,loader,lib,++count);
                /* removes 'value'; keeps 'key' for next iteration */
                lua_pop(L, 1);
            }
            lua_settop(L,0);

        }
        
    }

}


