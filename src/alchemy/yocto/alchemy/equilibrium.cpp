#include "yocto/alchemy/equilibrium.hpp"

namespace yocto
{
    namespace alchemy
    {
        equilibrium:: actor:: ~actor() throw()
        {
        }

        equilibrium:: actor::actor(const species::pointer &the_sp, const int the_nu ) throw() :
        sp(the_sp),
        nu(the_nu),
        next(0),
        prev(0)
        {
        }

        equilibrium:: actor:: actor( const actor &other ) throw() :
        sp(other.sp),
        nu(other.nu),
        next(0),
        prev(0)
        {
        }
        

    }
}

#include "yocto/exception.hpp"

namespace yocto
{
    namespace alchemy
    {

        
        equilibrium:: ~equilibrium() throw()
        {
        }

        equilibrium:: equilibrium(const string           &the_name,
                                  const library::pointer &the_lib,
                                  const Constant         &the_K) :
        name(the_name),
        lib(the_lib),
        K(the_K)
        {
        }

        void equilibrium:: add(const string &name, const int nu)
        {
            const species::pointer &sp = (*lib)[name];
            if(0==nu) return;
            
            for(const actor *node = products.head;node;node=node->next)
            {
                if(node->sp->name == sp->name)
                {
                    throw exception("<%s>: already got product '%s'", name.c_str(), sp->name.c_str());
                }
            }

            for(const actor *node = reactants.head;node;node=node->next)
            {
                if(node->sp->name == sp->name)
                {
                    throw exception("<%s>: already got reactant '%s'", name.c_str(), sp->name.c_str());
                }
            }

            actor *a = new actor(sp,nu);
            if(nu>0) products.push_back(a); else reactants.push_back(a);
        }


    }
}
