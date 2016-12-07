#include "yocto/alchemy/equilibrium.hpp"

namespace yocto
{
    namespace alchemy
    {
        actor:: ~actor() throw()
        {
        }

        actor::actor(const species::pointer &the_sp, const int the_nu ) throw() :
        sp(the_sp),
        nu(the_nu),
        next(0),
        prev(0)
        {
        }

        actor:: actor( const actor &other ) throw() :
        sp(other.sp),
        nu(other.nu),
        next(0),
        prev(0)
        {
        }
        

    }
}

namespace yocto
{
    namespace alchemy
    {
        true_equilibrium_constant:: true_equilibrium_constant(const double K) throw() : value(K) {}
        true_equilibrium_constant:: ~true_equilibrium_constant() throw() {}
        true_equilibrium_constant:: true_equilibrium_constant(const true_equilibrium_constant &other) throw() : value(other.value) {}
        double true_equilibrium_constant:: operator()(double) throw() { return value; }

        equilibrium_constant_callable * true_equilibrium_constant::clone() const
        {
            return new true_equilibrium_constant(*this);
        }

        equilibrium_constant_callable * true_equilibrium_constant:: create(const double K)
        {
            return new true_equilibrium_constant(K);
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

        equilibrium:: equilibrium(const string               &the_name,
                                  const library::pointer     &the_lib,
                                  const equilibrium_constant &the_K) :
        name(the_name),
        lib(the_lib),
        K(the_K)
        {
        }

        equilibrium:: equilibrium(const string               &the_name,
                                  const library::pointer     &the_lib,
                                  const double                the_K) :
        name(the_name),
        lib(the_lib),
        K( true_equilibrium_constant::create(the_K) )
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
