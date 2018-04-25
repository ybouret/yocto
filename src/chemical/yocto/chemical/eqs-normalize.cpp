#include "yocto/chemical/equilibria.hpp"
#include "yocto/math/core/lu.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/core/svd.hpp"
#include "yocto/math/opt/bracket.hpp"
#include "yocto/math/opt/minimize.hpp"

namespace yocto
{
    using namespace math;
    
    namespace chemical
    {
        static const char fn[] = "equilibria.normalize: ";
        
        double equilibria:: callGamma(double alpha)
        {
            for(size_t j=M;j>0;--j)
            {
                const double Cj = (Ctry[j]=C[j]+alpha*dC[j]);
                if(active[j]&&(Cj<0))
                {
                    Ctry[j] = 0;
                }
            }
            updateGamma(Ctry);
            return GammaToScalar();
        }

        
        void equilibria:: normalize(array<double> &C0, const double t)
        {
            //__________________________________________________________________
            //
            //
            // initialize
            //
            //__________________________________________________________________
          
            //__________________________________________________________________
            //
            // check and transfer concentration
            //__________________________________________________________________
            for(size_t j=M;j>0;--j)
            {
                const double Cj = (C[j] = C0[j]);
                if(active[j]&&(Cj<0))
                {
                    throw exception("%s: invalid initial concentraton",fn);
                }
            }
            
            //__________________________________________________________________
            //
            // initialize Gamma, Phi, K at time t, and |Gamma|
            //__________________________________________________________________
            initializeGammaAndPhi(C,t);
            double GS = GammaToScalar();
            size_t cycle=0;
            while(true)
            {
                //______________________________________________________________
                //
                // compute full extent and Newton's step
                //______________________________________________________________
                ++cycle;
                std::cerr << "C0=" << C    << std::endl;
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
                    tao::setprobe(Ctry, C, alpha, dC);
                    Ctry[zindx] = 0;
                }
                else
                {
                    // full step!
                    tao::setsum(Ctry, C, dC);
                }
                
                
                //______________________________________________________________
                //
                // numerical rounding
                //______________________________________________________________
                for(size_t j=M;j>0;--j)
                {
                    const double Cj = Ctry[j];
                    if(active[j]&&(Cj<0))
                    {
                        Ctry[j] = 0;
                    }
                }
                
                std::cerr << "C1=" << Ctry << std::endl;
                updateGammaAndPhi(Ctry);
                double gs = GammaToScalar();
                std::cerr << "GS: " << GS << " => " << gs << std::endl;
                
                //______________________________________________________________
                //
                // be sure to decrease |Gamma|
                //______________________________________________________________
                if(gs>GS)
                {
                    std::cerr << "Local Increase, alpha=" << alpha << std::endl;
                    numeric<double>::function F(this, & equilibria::callGamma);
                    std::cerr << "F(" << alpha << ")=" << F(alpha) << std::endl;
                    triplet<double> XX = { 0,  alpha, alpha };
                    triplet<double> FF = { GS, gs,    gs    };
                    //std::cerr << std::scientific;
                    std::cerr << "init:" << std::endl;
                    std::cerr << "\t" << XX << std::endl;
                    std::cerr << "\t" << FF << std::endl;
                    bracket<double>::inside(F, XX, FF);
                    std::cerr << "bracket:" << std::endl;
                    std::cerr << "\t" << XX << std::endl;
                    std::cerr << "\t" << FF << std::endl;
                    std::cerr << "xx=" << XX.b << std::endl;
                    minimize(F, XX, FF,0.0);
                    std::cerr << "minimize:" << std::endl;
                    std::cerr << "\t" << XX << std::endl;
                    std::cerr << "\t" << FF << std::endl;
                    alpha = XX.b;
                    gs    = F(alpha);
                    std::cerr << "alpha=" << alpha << std::endl;
                    std::cerr << "gs   =" << gs    << "/" << GS <<  std::endl;
                }
                
                //______________________________________________________________
                //
                // update C
                //______________________________________________________________
                tao::set(C,Ctry);
                if(gs>=GS||gs<=0)
                {
                    std::cerr << "C =" << C << std::endl;
                    break;
                }
                
                //if(cycle>30) break;
                GS  = gs;
            }
            
            std::cerr << "Gamma=" << Gamma << std::endl;
            
            for(size_t j=M;j>0;--j)
            {
                C0[j] = C[j];
            }
            
            
        }
    }
    
}

