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


namespace yocto
{
    namespace alchemy
    {

        static inline
        void __add_specific_constraint(Lua::State::Pointer &vm,
                                       const char          *name,
                                       boot                &loader,
                                       const int            count)
        {
            assert(vm->isstring(-1));
            const string label = vm->tostring(-1);
            vm->pop(1);

            if( "E/N" == label )
            {
                std::cerr << "Electroneutrality" << std::endl;
                loader.electroneutrality();
                goto DISCARD;
            }


            if( "OSM" == label )
            {
                std::cerr << "Enforcing osmolarity" << std::endl;
                if(!vm->next(-2))
                {
                    throw exception("%s, constraint#%d: missing osmolarity value", name, count);
                }
                if(!vm->isNumber(-1))
                {
                    throw exception("%s, constraint#%d: osmolarity value is not a number", name, count);
                }
                const double osm = vm->tonumber(-1);
                std::cerr << "\tto OSM=" << osm << std::endl;
                loader.osmolarity(osm);
                vm->pop(1);
                goto DISCARD;
            }

            if( "I" == label )
            {
                std::cerr << "Enforcing ionic strength" << std::endl;
                if(!vm->next(-2))
                {
                    throw exception("%s, constraint#%d: missing ionic strength value", name, count);
                }
                if(!vm->isNumber(-1))
                {
                    throw exception("%s, constraint#%d: ionic strength value is not a number", name, count);
                }
                const double I = vm->tonumber(-1);
                std::cerr << "\tto I=" << I << std::endl;
                loader.ionic_strength(I);
                vm->pop(1);
                goto DISCARD;
            }


            throw exception("%s, constraint#%d: unknown specific constraint '%s'", name, count, label.c_str());

            // discard remaining items
        DISCARD:
            while(vm->next(-2))
            {

                vm->pop(1);
            }
        }

        static inline
        void __add_component(Lua::State::Pointer &vm,
                             const char          *name,
                             const int            count,
                             boot::constraint    &cc,
                             const int            item)
        {
            assert(vm->istable(-1));
            // weight
            vm->pushnil();
            if(!vm->next(-2))
            {
                throw exception("%s, constraint#%d, component%d: missing weight", name,count,item);
            }
            if(!vm->isNumber(-1))
            {
                throw exception("%s, constraint#%d, component%d: weight is not a number", name,count,item);
            }
            const double weight = vm->tonumber(-1);
            vm->pop(1);

            // name
            if(!vm->next(-2))
            {
                throw exception("%s, constraint#%d, component%d: missing species", name,count,item);
            }
            if(!vm->isstring(-1))
            {
                throw exception("%s, constraint#%d, component%d: species is not a string", name,count,item);
            }
            const string sp_name = vm->tostring(-1);
            vm->pop(1);

            // discard end of table
            while(vm->next(-2)) vm->pop(1);

            std::cerr << "\t\tadding '" << sp_name << "', weight=" << weight << std::endl;
            cc.add(sp_name, weight);


        }


        static inline
        void __add_generic_constraint(Lua::State::Pointer &vm,
                                      const char          *name,
                                      boot                &loader,
                                      const int            count)
        {
            assert(vm->isNumber(-1));

            const double value = vm->tonumber(-1);
            vm->pop(1);
            std::cerr << "\t\tvalue=" << value << std::endl;
            boot::constraint &cc = loader.create(value);

            // then parse coeff/species
            int item = 0;
            while(vm->next(-2))
            {
                ++item;
                std::cerr << "\t\tadding component#" << item << std::endl;
                if(!vm->istable(-1))
                {
                    throw exception("%s, constraint#%d: component#%d is not a coeff/species table",name,count,item);
                }
                __add_component(vm, name, count,cc,item);
                vm->pop(1);
            }

        }

        void __add_constraint(Lua::State::Pointer &vm,
                              const char          *name,
                              boot                &loader,
                              const int            count)
        {
            assert(vm->istable(-1));
            std::cerr << "\tconstraint #" << count << std::endl;

            vm->pushnil();
            if(!vm->next(-2))
            {
                throw exception("%s, constraint#%d is empty",name,count);
            }


            if( LUA_TSTRING == vm->type(-1) )
            {
                __add_specific_constraint(vm,name,loader,count);
                return;
            }


            if(vm->isNumber(-1))
            {
                __add_generic_constraint(vm,name,loader,count);
                return;
            }

            throw exception("%s, constraint #%d must start with string or number",name,count);
        }

        void __lua:: load(Lua::State::Pointer &vm,
                          const string        &bootName,
                          boot                &loader)
        {
            const char *name = bootName.c_str();
            vm->settop(0);
            vm->getglobal(name);
            if(!vm->istable(-1))
            {
                throw exception("'%s' is not a lua table", name);
            }
            const size_t n = vm->GetTableLength();
            std::cerr << "Parsing " << n << " constraints from " << name << std::endl;
            
            // iterating over each equilibrium
            vm->pushnil();  /* first key */
            int count = 0;
            while (vm->next(-2) != 0)
            {
                ++count;
                if(!vm->istable(-1))
                {
                    throw exception("%s: constraint#%d is not a table", name, count);
                }
                __add_constraint(vm,name,loader,count);
                vm->pop(1);
            }
            
            
        }
    }
}

