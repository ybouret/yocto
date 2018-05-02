#include "yocto/chemical/equilibrium.hpp"
#include "yocto/exception.hpp"
#include "yocto/code/ipower.hpp"
#include "yocto/math/core/tao.hpp"
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
            actors             *l   = NULL;
            actor              *a   = NULL;
            
            for(actor *r=reactants.head;r;r=r->next)
            {
                if(id==r->sp->name)
                {
                    a=r;
                    l=&reactants;
                    goto DISPATCH;
                }
            }
            
            for(actor *p=products.head;p;p=p->next)
            {
                if(id==p->sp->name)
                {
                    a=p;
                    l=&products;
                    goto DISPATCH;
                }
            }
            
            
        DISPATCH:
            if(!a)
            {
                // actor doesn't exist
                if(!nu)
                {
                    return false;
                }
                else
                {
                    a = new actor(sp,nu);
                    if(nu>0)
                    {
                        products.push_back(a);
                    }
                    else
                    {
                        reactants.push_back(a);
                    }
                    return true;
                }
                
            }
            else
            {
                // actor already exists
                assert(l!=NULL);
                const int new_nu = a->nu + nu;
                if(!new_nu)
                {
                    // removed...
                    delete l->unlink(a);
                    return false;
                }
                else
                {
                    if(new_nu*nu>0)
                    {
                        // same sign
                        (int &)(a->nu) = new_nu;
                    }
                    else
                    {
                        // change side
                        actors *t = (new_nu>0) ? &products : &reactants;
                        assert(t!=l);
                        t->push_back( l->unlink(a) );
                    }
                    return true;
                }
            }
            
        }
        
        void equilibrium:: display( std::ostream &os, const size_t ns) const
        {
            const equilibrium &eq = *this;
            os << eq.name << ':';
            for(size_t i=0;i<=ns;++i)
            {
                os << ' ';
            }
            {
                size_t ir = 1;
                for( const actor *r=eq.reactants.head;r;r=r->next,++ir)
                {
                    assert(r->nu<0);
                    if(ir>1)
                    {
                        os << " + ";
                    }
                    if(r->nu<-1)
                    {
                        os << -r->nu << '*';
                    }
                    os << r->sp->name;
                }
            }
            os << " <=> ";
            {
                size_t ip = 1;
                for( const actor *p=eq.products.head;p;p=p->next,++ip)
                {
                    assert(p->nu>0);
                    if(ip>1)
                    {
                        os << " + ";
                    }
                    if(p->nu>1)
                    {
                        os << p->nu << '*';
                    }
                    os << p->sp->name;
                }
            }
            os << '\t' << '(' << eq.K(0) << ')';
        }

        
        std::ostream & operator<<( std::ostream &os, const equilibrium &eq)
        {
            eq.display(os,0);
            return os;
        }

        void equilibrium:: check() const
        {
            int z = 0;
            for(const actor *a=reactants.head;a;a=a->next)
            {
                z += a->nu * a->sp->z;
            }
            for(const actor *a=products.head;a;a=a->next)
            {
                z += a->nu * a->sp->z;
            }
            if(z!=0)
            {
                throw exception("equilibrium '%s' doesn't conserve charge!", *name);
            }
        }

        int    equilibrium::productsStoichiometry() const throw()
        {
            int ans = 0;
            for(const actor *p=products.head;p;p=p->next)
            {
                ans += p->nu; assert( p->nu>0 );
            }
            return ans;
        }


        void equilibrium:: fill( array<double> &nu, array<bool> &active) const throw()
        {
            assert(nu.size()==active.size());
            for(const actor *a=reactants.head;a;a=a->next)
            {
                const size_t i = a->sp->indx;
                nu[i]     = a->nu;
                active[i] = true;
            }
            for(const actor *a=products.head;a;a=a->next)
            {
                const size_t i = a->sp->indx;
                nu[i]     = a->nu;
                active[i] = true;
            }
            
            
        }

        double equilibrium:: computeGamma( const array<double> &C, const double Kt ) const
        {
            double lhs = 1;
            {
                for(const actor *r=reactants.head;r;r=r->next)
                {
                    assert(r->nu<0);
                    lhs *= ipower( C[r->sp->indx], r->ev );
                }
            }

            double rhs = 1;
            {
                for(const actor *p=products.head;p;p=p->next)
                {
                    assert(p->nu>0);
                    rhs *= ipower( C[p->sp->indx], p->ev );
                }
            }


            return lhs*Kt - rhs;
        }

        void   equilibrium:: computeGradient( array<double> &Phi, const array<double> &C, const double Kt) const
        {
            math::tao::ld(Phi,0);
            for(const actor *a = reactants.head; a != NULL; a=a->next )
            {
                const  size_t j     = a->sp->indx;
                double        lhs   = Kt * a->ev * ipower(C[j],a->evm);

                for(const actor *b = reactants.head; b!=NULL; b=b->next)
                {
                    if(a!=b)
                    {
                        lhs *= ipower(C[b->sp->indx],b->ev);
                    }
                }

                Phi[j] = lhs;
            }

            for(const actor *a = products.head; a != NULL; a=a->next )
            {
                const  size_t    j   = a->sp->indx;
                double           rhs = a->ev * ipower(C[j],a->evm);

                for(const actor *b = products.head; b!=NULL; b=b->next)
                {
                    if(a!=b)
                    {
                        rhs *= ipower(C[b->sp->indx],b->ev);
                    }
                }

                Phi[j] = -rhs;
            }

        }

        int   equilibrium:: check_ranges(range &fwd,
                                         range &rev,
                                         const array<double> &C ) const throw()
        {
            int status = has_none;
            
            fwd.exists = false;
            fwd.extent = 0;
            fwd.index  = 0;
            for(const actor *a = reactants.head;a;a=a->next)
            {
                const int    nu  = a->nu; assert(nu<0);
                const size_t id  = a->sp->indx;
                const double c   = C[id];
                const bool   ok  = (c>=0);
                const double xi  = (ok?c/(-nu):0);
                if(!fwd.exists)
                {
                    fwd.exists=true;
                    fwd.extent=xi;
                    if(ok)
                    {
                        fwd.index =id;
                    }
                }
                else
                {
                    if(xi<fwd.extent)
                    {
                        fwd.extent = xi;
                        if(ok)
                        {
                            fwd.index=id;
                        }
                    }
                }
            }

            rev.exists = false;
            rev.extent = 0;
            rev.index  = 0;
            for(const actor *a = products.head;a;a=a->next)
            {
                const int    nu  = a->nu; assert(nu>0);
                const size_t id  = a->sp->indx;
                const double c   = C[id];
                const bool   ok  = (c>=0);
                const double xi  = (ok?-c/nu:0);
                if(!rev.exists)
                {
                    rev.exists=true;
                    rev.extent=xi;
                    if(ok)
                    {
                        rev.index =id;
                    }
                }
                else
                {
                    if(xi>rev.extent)
                    {
                        rev.extent = xi;
                        if(ok)
                        {
                            rev.index  = id;
                        }
                    }
                }
            }

            if(fwd.exists) status |= has_fwd;
            if(rev.exists) status |= has_rev;

            return status;
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

