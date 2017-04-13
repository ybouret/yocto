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

            //__________________________________________________________________
            //
            // Get the name of the species
            //__________________________________________________________________
            if(!vm.next(-2))
            {
                throw exception("%s#%d: missing species name",libname,count);
            }
            const string sp_name = vm.To<string>(-1);
            std::cerr << "name= " << sp_name << std::endl;
            vm.pop(1);


            //__________________________________________________________________
            //
            // Get the charge of the species
            //__________________________________________________________________
            if(!vm.next(-2))
            {
                throw exception("%s#%d: missing species charge",libname,count);
            }
            const int sp_z = vm.To<int>(-1);
            std::cerr << "z   = " << sp_z << std::endl;
            vm.pop(1);

            //__________________________________________________________________
            //
            // create it
            //__________________________________________________________________
            species &sp = lib.add(sp_name,sp_z);

            //__________________________________________________________________
            //
            // callback on other items
            //__________________________________________________________________
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

            //__________________________________________________________________
            //
            // iterating over each species
            //__________________________________________________________________
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




