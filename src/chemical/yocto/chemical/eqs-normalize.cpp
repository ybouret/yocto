#include "yocto/chemical/equilibria.hpp"
#include "yocto/math/core/lu.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/core/svd.hpp"
#include "yocto/math/opt/bracket.hpp"

namespace yocto
{
    using namespace math;
    
    namespace chemical
    {
        static const char fn[] = "equilibria.normalize: ";
        
        double equilibria:: callGamma(double alpha)
        {
            tao::setprobe(Ctry, C, alpha, dC);
            updateGamma(Ctry);
            return GammaToScalar();
        }

        
        void equilibria:: normalize(array<double> &C0, const double t)
        {
            //__________________________________________________________________
            //
            // initialize
            //__________________________________________________________________
            for(size_t j=M;j>0;--j)
            {
                const double Cj = (C[j] = C0[j]);
                if(active[j]&&(Cj<0))
                {
                    equilibrium::database      &db = *this;
                    const equilibrium::pointer &eq = db(j);
                    throw exception("%s: invalid initial [%s]=%g",fn, *(eq->name), Cj);
                }
            }
            
            initializeGammaAndPhi(C,t);
            double GS = GammaToScalar();
            size_t cycle=0;
            while(true)
            {
                //______________________________________________________________
                //
                // compute system and full extent
                //______________________________________________________________
                ++cycle;
                std::cerr << "C    =" << C    << std::endl;
                if(!computeW())
                {
                    throw exception("%ssingular system",fn);
                }
                
                tao::neg(xi,Gamma);
                LU<double>::solve(W,xi);
                tao::mul(dC,NuT,xi);
                
                //______________________________________________________________
                //
                // detect a limitation in full step
                //______________________________________________________________
                double zcoef = 0;
                size_t zindx = 0;
                
                
                for(size_t j=M;j>0;--j)
                {
                    const double d = dC[j];
                    if(active[j] && (d<0) )
                    {
                        const double c     = C[j]; assert(c>=0);
                        const double ztemp = c/(-d);
                        if(ztemp>1)
                        {
                            // is not a limitation
                            continue;
                        }
                        
                        if( (zindx<=0) || (ztemp<zcoef) )
                        {
                            zindx = j;
                            zcoef = ztemp;
                        }
                    }
                }
                
                //______________________________________________________________
                //
                // take a step, keep track of truncation
                //______________________________________________________________
                double  alpha = 1;
                if(zindx>0)
                {
                    assert(zcoef<=1);
                    alpha=zcoef;
                    std::cerr << "zindx=" << zindx << std::endl;
                    std::cerr << "alpha=" << alpha << std::endl;
                    assert(active[zindx]);
                    C[zindx] = 0;
                    tao::muladd(C,alpha,dC);
                }
                else
                {
                    // full step!
                    tao::add(C,dC);
                }
                
                
                // numerical rounding
                for(size_t j=M;j>0;--j)
                {
                    const double Cj = C[j];
                    if(active[j]&&(Cj<0))
                    {
                        C[j] = 0;
                    }
                }
                
                std::cerr << "C1=" << C << std::endl;
                updateGammaAndPhi(C);
                const double gs = GammaToScalar();
                std::cerr << "GS: " << GS << " => " << gs << std::endl;
                
                //______________________________________________________________
                //
                // be sure to decrease |Gamma|
                //______________________________________________________________
                if(gs>GS)
                {
                    std::cerr << "Local Increase" << std::endl;
                    numeric<double>::function F(this, & equilibria::callGamma);
                    std::cerr << "F(" << alpha << ")=" << F(alpha) << std::endl;
                    triplet<double> XX = { 0,  alpha, alpha };
                    triplet<double> FF = { GS, gs,    gs    };
                    bracket<double>::inside(F, XX, FF);
                    std::cerr << "bracket:" << std::endl;
                    std::cerr << "\t" << XX << std::endl;
                    std::cerr << "\t" << FF << std::endl;
                    exit(0);
                }
                
                
                if(cycle>30) break;
                GS  = gs;
            }
            
            
            
            for(size_t j=M;j>0;--j)
            {
                C0[j] = C[j];
            }
            
            
        }
    }
    
}

