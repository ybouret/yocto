#include "yocto/chemical/equilibria.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/core/adjoint.hpp"

namespace yocto
{
    using namespace math;
    
    namespace chemical
    {


        bool equilibria:: apply_extent() throw()
        {

            //__________________________________________________________________
            //
            // we start from C: analyze and clip extent
            //__________________________________________________________________
            std::cerr << "xi0=" << xi << std::endl;
            vector<bool>       nil(M,false);
            size_t i=1;
            for(iterator ii=begin();ii!=end();++i,++ii)
            {
                const equilibrium &eq     = **ii;
                double            &extent = xi[i];
                eq.check_extent(extent,C);
                std::cerr << std::endl;
            }

            std::cerr << "xi1=" << xi << std::endl;
            std::cerr << "nil=" << nil << std::endl;
            return false;
        }

        bool equilibria:: balance() throw()
        {

            size_t nbad  = 0;
            for(size_t j=M;j>0;--j)
            {
                beta[j] = 0;
                const double Cj = C[j];
                if(active[j]&&(Cj<0))
                {
                    beta[j] = -Cj;
                    ++nbad;
                }
            }
            if(nbad<=0)
            {
                std::cerr << "balanced" << std::endl;
                return true;
            }
            else
            {
                std::cerr << "C   =" << C << std::endl;
                std::cerr << "beta=" << beta << std::endl;
                tao::mul_and_div(xi,AdjNu,beta,Det);
                (void)apply_extent();

                return false;
            }



#if 0
            equilibrium::range fwd,rev;

            {
                size_t ii=1;
                for(iterator i=begin();i!=end();++i,++ii)
                {
                    const equilibrium &eq = **i;
                    eq.check_ranges(fwd,rev,C);
                    std::cerr << eq.name;
                    spaces_for(eq.name,std::cerr);
                    std::cerr << ": fwd=" << fwd << ", rev=" << rev << std::endl;
                }
            }



            size_t nbad  = 0;
            for(size_t j=M;j>0;--j)
            {
                beta[j] = 0;
                const double Cj = C[j];
                if(active[j]&&(Cj<0))
                {
                    beta[j] = -Cj;
                    ++nbad;
                }
            }
            if(nbad<=0)
            {
                std::cerr << "balanced" << std::endl;
                return true;
            }
            else
            {
                std::cerr << "C   =" << C << std::endl;
                std::cerr << "beta=" << beta << std::endl;
                tao::mul_and_div(xi,AdjNu,beta,Det);
                std::cerr << "xi=" << xi << std::endl;

                // Analyzing extent...
                size_t i=1;
                for(iterator ii=begin();ii!=end();++i,++ii)
                {
                    const equilibrium &eq     = **ii;
                    const int          status = eq.check_ranges(fwd,rev,C);
                    double            &extent = xi[i];
                    switch(status)
                    {
                        case equilibrium::has_fwd:
                            break;

                        case equilibrium::has_rev:
                            break;

                        case equilibrium::has_both:
                            break;

                        default:
                            assert(equilibrium::has_none==status);
                            break;
                    }
                }

                tao::mul(dC,NuT,xi);
                std::cerr << "dC=" << dC << std::endl;
                return false;
            }


#endif
        }
        
    }

}

