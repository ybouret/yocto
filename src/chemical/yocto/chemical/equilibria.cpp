
#include "yocto/chemical/equilibria.hpp"
#include "yocto/exception.hpp"

namespace yocto
{
    namespace chemical
    {
        equilibria:: ~equilibria() throw() {}

        equilibria::equilibria(const string &id, const size_t n) :
        equilibrium::database(n,as_capacity),
        name(id),
        M(0),
        N(0)
        {
        }

        equilibrium & equilibria:: operator()(const string &id, const double K)
        {
            equilibrium::pointer eq( new constant_equilibrium(id,K) );
            if(!insert(eq))
            {
                throw exception("%s: multiple '%s'", *name, *id);
            }
            return *eq;
        }

        std::ostream & operator<<( std::ostream &os, const equilibria &eqs)
        {
            os << "<equilibria:" << eqs.name << "#" << eqs.size() << ">" << std::endl;
            for( equilibria::const_iterator i=eqs.begin();i!=eqs.end();++i)
            {
                os << '\t' << **i << std::endl;
            }
            os << "<equilibria:" << eqs.name << "/>";
            return os;
        }

        void equilibria::clear() throw()
        {
            (size_t &)M = 0;
            (size_t &)N = 0;

            xi.     release();
            Gam.    release();
            Phi.    release();
            nuT.    release();
            nu.     release();

            active. release();
            dC.     release();
            C.      release();
        }


        void equilibria:: compile_for(const library &lib)
        {
            clear();

            // check equilibria
            for(iterator i=begin();i!=end();++i)
            {
                (**i).check();
            }

            // compute sizes
            (size_t &)M = lib.size();
            (size_t &)N = size();
            if(M<=0) throw exception("empty library!");

            try
            {
                C.make(M);
                dC.make(M);
                active.make(M,false);

                if(N>0)
                {
                    nu. make(N,M);
                    nuT.make(M,N);
                    Phi.make(N,M);
                    Gam.make(N);
                    xi.make(N);
                    {
                        size_t ii=1;
                        for(iterator i=begin();i!=end();++i,++ii)
                        {
                            (**i).fill(nu[ii],active);
                        }
                    }
                    for(size_t i=1;i<=N;++i)
                    {
                        for(size_t j=1;j<=M;++j)
                        {
                            nuT[j][i] = nu[i][j];
                        }
                    }
                }


                
            }
            catch(...)
            {
                clear();
                throw;
            }
        }

    }
}
