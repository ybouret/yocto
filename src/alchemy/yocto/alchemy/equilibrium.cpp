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
        id(sp->indx),
        nu(the_nu),
        ev( (nu<0) ?-nu:nu),
        evm(ev-1),
        next(0),
        prev(0)
        {
        }

        actor:: actor( const actor &other ) throw() :
        sp(other.sp),
        id(other.id),
        nu(other.nu),
        ev(other.ev),
        evm(other.evm),
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

        typedef equilibrium_constant::callable  equilibrium_constant_callable;

        //! wrapper for true constant
        class true_equilibrium_constant : public equilibrium_constant_callable
        {
        public:
            explicit true_equilibrium_constant(const double K) throw();
            virtual ~true_equilibrium_constant() throw();
            true_equilibrium_constant(const true_equilibrium_constant &other) throw();
            const double value;
            double operator()(double) throw();

            virtual equilibrium_constant_callable * clone() const;
            static  equilibrium_constant_callable * create(const double K);

        private:
            YOCTO_DISABLE_ASSIGN(true_equilibrium_constant);
        };

        true_equilibrium_constant::  true_equilibrium_constant(const double K) throw() : value(K) {}
        true_equilibrium_constant:: ~true_equilibrium_constant() throw() {}
        true_equilibrium_constant::  true_equilibrium_constant(const true_equilibrium_constant &other) throw() : value(other.value) {}
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
#include "yocto/code/ipower.hpp"
#include "yocto/math/core/tao.hpp"

namespace yocto
{
    namespace alchemy
    {
        const actors & equilibrium:: get_products()  const throw()
        {
            return products;
        }
        
        const actors & equilibrium:: get_reactants()  const throw()
        {
            return reactants;
        }

        
        equilibrium:: ~equilibrium() throw()
        {
        }

        equilibrium:: equilibrium(const string               &the_name,
                                  const library::pointer     &the_lib,
                                  const equilibrium_constant &the_K) :
        name(the_name),
        pLib(the_lib),
        K(the_K),
        products(),
        reactants()
        {
        }

        equilibrium:: equilibrium(const string               &the_name,
                                  const library::pointer     &the_lib,
                                  const double                the_K) :
        name(the_name),
        pLib(the_lib),
        K( true_equilibrium_constant::create(the_K) )
        {

        }

        const string & equilibrium:: key() const throw() { return name; }


        static inline void show_actor(const actor *node, std::ostream &os, const actor *head, const bool is_product)
        {
            const int nu = is_product ? node->nu : -node->nu;
            switch(nu)
            {
                case  1: if(node!=head) { os << '+'; } break;
                case -1: os << '-'; break;
                default: os <<  nu << '*';  break;
            }
            os << '[' << node->sp->name << ']';
        }

        std::ostream & operator<<( std::ostream &os, const equilibrium &eq)
        {
            for(const actor *node = eq.reactants.head;node;node=node->next)
            {
                show_actor(node,os,eq.reactants.head,false);
            }

            os << " <=> ";

            for(const actor *node = eq.products.head;node;node=node->next)
            {
                show_actor(node,os,eq.products.head,true);
            }

            return os;
        }

        void equilibrium:: add(const string &name, const int nu)
        {
            const species::pointer &sp = (*pLib)[name];
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

        double equilibrium:: computeGamma(const array<double> &C,
                                          const double         t,
                                          double              &Kt) const throw()
        {
            return updateGamma(C,(Kt=K(t)));
        }

        double equilibrium:: updateGamma(const array<double> &C,
                                         const double         Kt) const throw()
        {
            assert(C.size()>=pLib->size());
            double lhs = Kt;
            double rhs = 1;
            for(const actor *a = reactants.head; a != NULL; a=a->next )
            {
                assert(a->nu<0);
                assert(a->ev>0);
                const double Ca = C[a->id];
                lhs *= ipower<double>(Ca,a->ev);
            }

            for(const actor *a = products.head; a != NULL; a=a->next )
            {
                assert(a->nu>0);
                assert(a->ev>0);
                const double Ca = C[a->id];
                rhs *= ipower<double>(Ca,a->ev);
            }

            return lhs - rhs;

        }

#define __ZPHI(INDEX) Phi[INDEX]=0

        void equilibrium:: computeGradient(array<double>       &Phi,
                                           const array<double> &C,
                                           const double         Kt) const throw()
        {
            assert(Phi.size()==pLib->size());
            assert(C.size()>=pLib->size());
            const size_t M = pLib->size();
            YOCTO_LOOP_FUNC(M,__ZPHI,1);

            for(const actor *a = reactants.head; a != NULL; a=a->next )
            {
                const  size_t j   = a->id;
                double        lhs = Kt * a->ev * ipower(C[j],a->evm);

                for(const actor *b = reactants.head; b!=NULL; b=b->next)
                {
                    if(a!=b)
                    {
                        lhs *= ipower(C[b->id],b->ev);
                    }
                }

                Phi[j] = lhs;
            }

            for(const actor *a = products.head; a != NULL; a=a->next )
            {
                const  size_t j   = a->sp->indx;
                double        rhs = a->ev * ipower(C[j],a->evm);

                for(const actor *b = products.head; b!=NULL; b=b->next)
                {
                    if(a!=b)
                    {
                        rhs *= ipower(C[b->id],b->ev);
                    }
                }

                Phi[j] = -rhs;
            }
        }

        
        double equilibrium:: dtGamma(const array<double> &C,
                                     const double         t,
                                     const double         dt,
                                     drvs_type           &drvs)
        {
            double lhs = drvs(K,t,dt);
            for(const actor *a = reactants.head; a != NULL; a=a->next )
            {
                assert(a->nu<0);
                assert(a->ev>0);
                const double Ca = C[a->id];
                lhs *= ipower<double>(Ca,a->ev);
            }
            return lhs;
        }

    }

}


