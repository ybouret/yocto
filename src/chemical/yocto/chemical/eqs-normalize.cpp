#include "yocto/chemical/equilibria.hpp"
#include "yocto/math/core/lu.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/core/svd.hpp"

namespace yocto
{
    using namespace math;
    
    namespace chemical
    {
        static const char fn[] = "equilibria.normalize: ";
        
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
                std::cerr << "Gamma=" << Gamma << std::endl;
                std::cerr << "Phi  =" << Phi   << std::endl;
                tao::mmul_rtrn(W,Phi,Nu);
                std::cerr << "W=" << W << std::endl;
                if(!computeW())
                {
                    tao::mmul_rtrn(W,Phi,Nu);
                    std::cerr << "W=" << W << std::endl;
                    matrix<double> V(N,N);
                    vector<double> w(N);
                    if(!svd<double>::build(W, w, V))
                    {
                        std::cerr << "can't event build SVD!" << std::endl;
                        throw exception("%ssingular system",fn);
                    }
                    std::cerr << "w=" << w << std::endl;
                    const size_t dimKer = svd<double>::truncate(w);
                    std::cerr << "w=" << w << ", dimKer=" << dimKer << std::endl;
                    svd<double>::solve(W,w,V,Gamma,xi);
                    tao::neg(xi,xi);
                    tao::mul(dC,NuT,xi);
                    std::cerr << "dC_svd=" << dC << std::endl;
                    throw exception("%ssingular system",fn);
                }
                else
                {
                    tao::neg(xi,Gamma);
                    LU<double>::solve(W,xi);
                    tao::mul(dC,NuT,xi);
                    std::cerr << "dC_lu  =" << dC   << std::endl;
                }
                
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
                    exit(0);
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

