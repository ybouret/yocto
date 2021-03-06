#include "yocto/alchemy/lua/interface.hpp"

namespace yocto
{
    namespace alchemy
    {

        static inline
        void __add_item(Lua::State::Pointer &vm,
                        equilibrium         &eq,
                        const int            item)
        {
            assert(vm->istable(-1));
            vm->pushnil();

            //__________________________________________________________________
            //
            // Getting nu
            //__________________________________________________________________
            if(!vm->next(-2))
            {
                throw exception("%s, item#%d: missing coefficient", eq.name.c_str(),item);

            }
            if(!vm->isinteger(-1))
            {
                throw exception("%s, item#%d: coefficient is not an integer", eq.name.c_str(),item);
            }
            const int nu = vm->tointeger(-1);
            if(!nu) throw exception("%s, item#%d: coefficient is ZERO!", eq.name.c_str(),item);
            vm->pop(1);


            if(!vm->next(-2))
            {
                throw exception("%s, item#%d: missing species name", eq.name.c_str(),item);
            }
            if(!vm->isstring(-1))
            {
                throw exception("%s, itemd%d: species name is not a string",eq.name.c_str(),item);
            }
            const string id = vm->tostring(-1);
            vm->pop(1);

            if(vm->next(-2))
            {
                throw exception("%s, item#%d: unexpected extra data", eq.name.c_str(),item);
            }

            std::cerr << "nu=" << nu << " for " << id << std::endl;

            eq.add(id,nu);

        }


        static inline
        void __add_equilibria(Lua::State::Pointer &vm,
                              const char          *eqsName,
                              equilibria          &eqs,
                              const int            count)
        {
            assert(vm->istable(-1));

            vm->pushnil();
            //__________________________________________________________________
            //
            // equilibrium name
            //__________________________________________________________________

            if(!vm->next(-2))
            {
                throw exception("%s#%d: missing equilibrium name",eqsName,count);
            }
            const string eq_name = vm->To<string>(-1);
            std::cerr << "name= " << eq_name << std::endl;
            vm->pop(1);

            //__________________________________________________________________
            //
            // eq constant
            //__________________________________________________________________
            if(!vm->next(-2))
            {
                throw exception("%s#%d: missing equilibrium constant",eqsName,count);
            }

            bool has_constant = false;

            //__________________________________________________________________
            //
            // is a number, then it's a real constant
            //__________________________________________________________________
            if( !has_constant && vm->isNumber(-1) )
            {
                (void)eqs.add(eq_name, vm->tonumber(-1));
                has_constant = true;
            }

            //__________________________________________________________________
            //
            // time dependant constant
            //__________________________________________________________________
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

            //__________________________________________________________________
            //
            // will parse equilibrium topologie
            //__________________________________________________________________

            // fetch the create equilibrium
            equilibrium::pointer *ppEq = eqs.search(eq_name);
            if(!ppEq) throw exception("unexpected missing %s in equilibria!", eq_name.c_str());
            equilibrium &eq = **ppEq;
            std::cerr << "building " << eq.name << std::endl;
            std::cerr << "K(0)=    " << eq.K(0) << std::endl;


            //__________________________________________________________________
            //
            // parse nu/species pairs
            //__________________________________________________________________
            int item = 0;
            while(vm->next(-2))
            {
                ++item;
                if( !vm->istable(-1) )
                {
                    throw exception("%s: item#%d is not a table",eq_name.c_str(),item);
                }
                __add_item(vm,eq,item);
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
