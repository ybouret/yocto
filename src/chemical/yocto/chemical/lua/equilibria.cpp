#include "yocto/chemical/lua.hpp"
#include "yocto/ptr/auto.hpp"
#include "yocto/lua/lua-funcs.hpp"

namespace yocto
{
    namespace chemical
    {

        namespace
        {
            class lua_equilibrium : public equilibrium
            {
            public:
                inline explicit lua_equilibrium( const string &id, Lua::State::Pointer &vm, const string &fcn) :
                equilibrium(id),
                F(vm,fcn)
                {
                }

                inline virtual ~lua_equilibrium() throw()
                {
                }

                mutable Lua::Function<double> F;

            private:
                virtual double getK(double t) const
                {
                    return F(t);
                }

                YOCTO_DISABLE_COPY_AND_ASSIGN(lua_equilibrium);
            };
        }

        static inline
        void __add_equilibrium(Lua::State::Pointer &vm,
                               equilibria          &eqs,
                               const int            count,
                               library             &lib)
        {
            lua_State *L = **vm;
            assert(lua_istable(L,-1));
            const size_t n=vm->GetTableLength();
            if(n<3)
            {
                throw exception("in '%s': not enough items to parse equilibrium#%d", *(eqs.name), count);
            }

            // get eq name
            assert(lua_istable(L,-1));
            lua_rawgeti(L,-1,1);
            if(!lua_isstring(L,-1)) throw exception("in '%s': invalid equilibrium#%d name", *(eqs.name), count );
            const string name = lua_tostring(L,-1);
            lua_pop(L,1);

            // get constant
            assert(lua_istable(L,-1));
            lua_rawgeti(L,-1,2);
            const int K_type = lua_type(L,-1);
            auto_ptr<equilibrium> pEq;

            switch( lua_type(L,-1) )
            {
                case LUA_TNUMBER: {
                    const double K = lua_tonumber(L,-1);
                    std::cerr << "...new constant equilibrium: " << name << ", K=" << K << std::endl;
                    pEq.reset( new constant_equilibrium(name,K) );
                } break;

                case LUA_TSTRING: {
                    const string K = lua_tostring(L,-1);
                    std::cerr << "...new function equilibrium: " << name << ", K=" << K << std::endl;
                    pEq.reset( new lua_equilibrium(name,vm,K) );
                } break;
                default:
                    throw exception("in '%s': invalid type '%s' for '%s' constant", *(eqs.name), lua_typename(L,K_type), *name);
            }
            lua_pop(L,1);
            assert( pEq.is_valid() );

            // then parse actors
            for(size_t i=3;i<=n;++i)
            {
                assert(lua_istable(L,-1));
                const unsigned j = (i-2);
                lua_rawgeti(L,-1,i);
                if(!lua_istable(L,-1)) throw exception("%s: actor#%u is not a table", *name, j);

                // get nu
                lua_rawgeti(L,-1,1);
                if(!lua_isnumber(L,-1)) throw exception("%s: actor#%u first item is not a number", *name, j);
                const int nu = int( lua_tointeger(L,-1) );
                lua_pop(L,1);

                // get species id
                assert(lua_istable(L,-1));
                lua_rawgeti(L,-1,2);
                if(!lua_isstring(L,-1)) throw exception("%s: actor#%d second item it not a string", *name, j);
                const string id = lua_tostring(L,-1);
                lua_pop(L,1);


                // register
                species &sp = lib(id);
                (void) (*pEq)(sp,nu);
                lua_pop(L,1);
            }



            // and insert into eqs
            const equilibrium::pointer p( pEq.yield() );
            if( !eqs.insert(p) )
            {
                throw exception("multiple equilibrium '%s' in '%s'", *name, *(eqs.name) );
            }


        }

        void __lua:: load( Lua::State::Pointer &vm, equilibria &eqs, library &lib)
        {
            std::cerr << "lua: loading equilibria " << eqs.name << std::endl;
            lua_State *L = **vm;
            lua_settop(L,0);
            const char *name = *(eqs.name);
            lua_getglobal(L,name);
            if(!lua_istable(L,-1))
            {
                throw exception("couldn't find equilibria '%s'", name);
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
                __add_equilibrium(vm,eqs,++count,lib);
                lua_pop(L, 1);
            }
            lua_settop(L,0);

            eqs.compile_for(lib);

        }

    }

}


