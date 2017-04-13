#include "yocto/alchemy/lua/interface.hpp"


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
