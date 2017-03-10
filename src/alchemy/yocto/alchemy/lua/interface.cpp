#include "yocto/alchemy/lua/interface.hpp"

namespace yocto
{
    namespace alchemy
    {

        static inline void __add_species(Lua::State &vm,
                                         const char *libname,
                                         library    &lib,
                                         const int   count,
                                         __lua::species_callback *cb)
        {
            assert(vm.istable(-1));
            vm.pushnil();
            if(!vm.next(-2))
            {
                throw exception("%s#%d: missing species name",libname,count);
            }
            const string sp_name = vm.To<string>(-1);
            std::cerr << "name= " << sp_name << std::endl;
            vm.pop(1);


            if(!vm.next(-2))
            {
                throw exception("%s#%d: missing species charge",libname,count);
            }
            const int sp_z = vm.To<int>(-1);
            std::cerr << "z   = " << sp_z << std::endl;
            vm.pop(1);

            species &sp = lib.add(sp_name,sp_z);

            while(vm.next(-2))
            {
                if(cb)
                {
                    (*cb)(vm,sp);
                }
                vm.pop(1);
            }



        }

        void __lua:: load(Lua::State::Pointer &vm,
                          const string        &libName,
                          library             &lib,
                          species_callback    *cb)
        {
            const char *name = libName.c_str();
            vm->settop(0);
            vm->getglobal(name);
            if(!vm->istable(-1))
            {
                throw exception("'%s' is not a lua table", name);
            }
            const size_t n = vm->GetTableLength();
            std::cerr << "Parsing " << n << " species from " << name << std::endl;

            // iterating over each species
            vm->pushnil();  /* first key */
            int count = 0;
            while (vm->next(-2) != 0) {
                /* uses 'key' (at index -2) and 'value' (at index -1) */
#if 0
                printf("%s - %s\n",
                       vm.type_name_at(-2),
                       vm.type_name_at(-1)
                       //lua_typename(*vm, lua_type(*vm, -2)),
                       //lua_typename(*vm, lua_type(*vm, -1))
                       );
#endif
                ++count;
                if(!vm->istable(-1))
                {
                    throw exception("%s: entry #%d is not a table", name, count);
                }
                __add_species(*vm,name,lib,count,cb);

                /* removes 'value'; keeps 'key' for next iteration */
                vm->pop(1);
            }


        }
    }

}

namespace yocto
{
    namespace alchemy
    {

        static inline
        void __add_equilibria(Lua::State::Pointer &vm,
                              const char          *eqsName,
                              equilibria          &eqs,
                              const int            count)
        {
            assert(vm->istable(-1));

            // eq name
            vm->pushnil();
            if(!vm->next(-2))
            {
                throw exception("%s#%d: missing equilibrium name",eqsName,count);
            }
            const string eq_name = vm->To<string>(-1);
            std::cerr << "name= " << eq_name << std::endl;
            vm->pop(1);

            // eq constant
            if(!vm->next(-2))
            {
                throw exception("%s#%d: missing equilibrium constant",eqsName,count);
            }

            bool has_constant = false;

            if( !has_constant && vm->isnumber(-1) )
            {
                (void)eqs.add(eq_name, vm->tonumber(-1));
                has_constant = true;
            }

            if( !has_constant && vm->isstring(-1) )
            {
                const string fn = vm->tostring(-1);
                Lua::Function<double> K_lua(vm,fn);
                equilibrium_constant  K(K_lua);
                (void)eqs.add(eq_name,K);
                has_constant = true;
            }

            if(!has_constant)
            {
                throw exception("%s: constant must be a number of a function", eq_name.c_str());
            }

            vm->pop(1);

            // will parse eq
            // fetch eq
            equilibrium::pointer *ppEq = eqs.search(eq_name);
            if(!ppEq) throw exception("unexpected missing %s in equilibria!", eq_name.c_str());
            equilibrium &eq = **ppEq;
            std::cerr << "building " << eq.name << std::endl;


            // discard end of table
            while(vm->next(-2))
            {

                vm->pop(1);
            }


        }


        void __lua:: load(Lua::State::Pointer &vm,
                          const string        &eqsName,
                          equilibria          &eqs)
        {
            const char *name = eqsName.c_str();
            vm->settop(0);
            vm->getglobal(name);
            if(!vm->istable(-1))
            {
                throw exception("'%s' is not a lua table", name);
            }
            const size_t n = vm->GetTableLength();
            std::cerr << "Parsing " << n << " equilibria from " << name << std::endl;

            // iterating over each equilibrium
            vm->pushnil();  /* first key */
            int count = 0;
            while (vm->next(-2) != 0) {
                /* uses 'key' (at index -2) and 'value' (at index -1) */
#if 0
                printf("%s - %s\n",
                       vm.type_name_at(-2),
                       vm.type_name_at(-1)
                       //lua_typename(*vm, lua_type(*vm, -2)),
                       //lua_typename(*vm, lua_type(*vm, -1))
                       );
#endif
                ++count;
                if(!vm->istable(-1))
                {
                    throw exception("%s: entry #%d is not a table", name, count);
                }
                __add_equilibria(vm,name,eqs, count);

                /* removes 'value'; keeps 'key' for next iteration */
                vm->pop(1);
            }

            std::cerr << eqs << std::endl;

        }
        
    }
}



