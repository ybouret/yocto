#include "yocto/chemical/equilibrium.hpp"
#include "yocto/exception.hpp"

namespace yocto
{
    namespace chemical
    {
        equilibrium:: ~equilibrium() throw()
        {
        }

        equilibrium:: equilibrium(const string &id) :
        name(id),
        K( this, & equilibrium:: getK )
        {
        }

        const string & equilibrium:: key() const throw()
        {
            return name;
        }

        bool equilibrium:: operator()(species &sp, const int nu)
        {
            const string       &id  = sp.name;
            component::pointer *pp  = content.search(id);
            if(!pp)
            {
                // create component
                if(!nu)
                {
                    dispatch();
                    return false;
                }
                else
                {
                    component::pointer p( new component(sp,nu) );
                    if( !content.insert(p) )
                    {
                        throw exception("equilibrium: unexpected failure to insert '%s'",*id);
                    }
                    dispatch();
                    return true;
                }
            }
            else
            {
                // update component
                component &cmp    = **pp;
                const int  new_nu = cmp.nu + nu;
                if(new_nu)
                {
                    (int&)(cmp.nu) = new_nu;
                    dispatch();
                    return true;
                }
                else
                {
                    if(!content.remove(id))
                    {
                        throw exception("equilibrium: failure to remove '%s'", *id);
                    }
                    dispatch();
                    return false;
                }
            }
        }

        void equilibrium:: dispatch()
        {
            products.free();
            reactants.free();
            products.ensure( content.size() );
            reactants.ensure( content.size() );
            for( components::iterator i=content.begin();i!=content.end();++i)
            {
                const component::pointer &p = *i;
                assert(p->nu!=0);
                if(p->nu>0)
                {
                    if( !products.insert(p) ) throw exception("%s: unexpected product failure for '%s'", *name, *(p->key()));
                }
                else
                {
                    assert(p->nu<0);
                    if( !reactants.insert(p) ) throw exception("%s: unexpected reactant failure for '%s'",*name, *(p->key()));
                }
            }
        }

        std::ostream & operator<<( std::ostream &os, const equilibrium &eq)
        {
            os << eq.name << ": ";
            {
                size_t ir = 1;
                for( components::const_iterator r=eq.reactants.begin();r!=eq.reactants.end();++r,++ir)
                {
                    const component &cmp = **r;
                    assert(cmp.nu<0);
                    if(ir>1)
                    {
                        os << " + ";
                    }
                    if(cmp.nu<-1)
                    {
                        os << -cmp.nu << '*';
                    }
                    os << cmp.key();
                }
            }
            os << " <=> ";
            {
                size_t ip = 1;
                for( components::const_iterator p=eq.products.begin();p!=eq.products.end();++p,++ip)
                {
                    const component &cmp = **p;
                    assert(cmp.nu>0);
                    if(ip>1)
                    {
                        os << " + ";
                    }
                    if(cmp.nu>1)
                    {
                        os << cmp.nu << '*';
                    }
                    os << cmp.key();
                }
            }
            os << ", " << eq.K(0);
            return os;
        }

        void equilibrium:: check() const
        {
            int z = 0;
            for(components::const_iterator i=content.begin();i!=content.end();++i)
            {
                const component &cmp = **i;
                z += cmp.nu * cmp.sp->z;
            }
            if(z!=0)
            {
                throw exception("equilibrium '%s' doesn't conserve charge!", *name);
            }
        }

        void equilibrium:: fill( array<double> &nu, array<bool> &active) const throw()
        {
            assert(nu.size()==active.size());
            for(components::const_iterator i=content.begin();i!=content.end();++i)
            {
                const component &cmp = **i;
                const species   &s   = *cmp.sp;
                assert(s.indx>0);
                assert(s.indx<=nu.size());
                assert(cmp.nu!=0);
                nu[s.indx] = cmp.nu;
                active[s.indx] = true;
            }
        }

    }
}

namespace yocto
{
    namespace chemical
    {
        constant_equilibrium:: constant_equilibrium(const string &id, const double Kvalue) :
        yocto::chemical::equilibrium(id),
        __K(Kvalue)
        {
        }


        constant_equilibrium:: ~constant_equilibrium() throw() {}

        double constant_equilibrium:: getK(double) const throw()
        {
            return __K;
        }
    }

}

