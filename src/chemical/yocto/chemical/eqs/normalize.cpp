#include "yocto/chemical/equilibria.hpp"
#include "yocto/math/core/lu.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/core/svd.hpp"
#include "yocto/math/opt/bracket.hpp"
#include "yocto/math/opt/minimize.hpp"
#include "yocto/math/types.hxx"

namespace yocto
{
    using namespace math;
    
    namespace chemical
    {

        bool equilibrium:: solve(array<double> &C, const double Kt) const
        {
            std::cerr << "Solving " << name << ", K=" << Kt << ", for C=" << C <<  std::endl;
            const double Gamma = computeGamma(C,Kt);
            std::cerr << "\tGamma=" << Gamma << std::endl;
            if(Gamma>0)
            {
                // looking for a positive extent
                range fwd;
                compute_forward(fwd,C);
                std::cerr << "\tfwd: " << fwd << std::endl;
                return false;
            }
            else if(Gamma<0)
            {
                // looking for a negative extent
                range rev;
                compute_reverse(rev,C);
                std::cerr << "\trev: " << rev << std::endl;
                return false;
            }
            else
            {
                return true;
            }
        }


        static const char fn[] = "equilibria.normalize: ";
        
        double equilibria:: __normGamma(double alpha)
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

        static inline
        void __optimize(math::numeric<double>::function &F,
                        triplet<double>                 &XX,
                        triplet<double>                 &FF) throw()
        {

            bracket<double>::inside(F, XX, FF);

            double width = XX.c-XX.a; assert(width>=0);
            while(true)
            {
                kernel::minimize(F, XX, FF);
                const double new_width = XX.c-XX.a;
                assert(new_width<=width);
                if(new_width>=width)
                {
                    break;
                }
                width = new_width;
            }

        }

        double equilibria:: __callG(double alpha)
        {
            assert(pEq!=NULL);
            tao::setprobe(Ctry, C,alpha, dC);
            return pEq->computeGamma(Ctry,KEq);
        }
        
        void equilibria:: compute_full_step()
        {
            std::cerr << "Phi=" << Phi << std::endl;
            tao::mmul_rtrn(W,Phi,Nu);
            std::cerr << "W=" << W << std::endl;
            if(!LU<double>::build(W))
            {
                throw exception("%ssingular set of concentrations",fn);
            }
            tao::neg(xi,Gamma);
            LU<double>::solve(W,xi);
            tao::mul(dC,NuT,xi);
            std::cerr << "xi  =" << xi << std::endl;
            std::cerr << "dC  =" << dC << std::endl;
        }


        void equilibria:: normalize(array<double> &C0, const double t)
        {
            // transfer
            for(size_t j=M;j>0;--j)
            {
                C[j] = C0[j];
                if(active[j]&&C[j]<0)
                {
                    throw exception("%sinvalid initial concentration",fn);
                }
            }
            

            // initialize K, Gamma, Phi, and GS
            initializeGammaAndPhi(C,t);
            
            while(true)
            {
                //______________________________________________________________
                //
                // at this point, C is valid, Gamma and Phi are computed@C
                //______________________________________________________________
                std::cerr << "Cini =" << C << std::endl;
                std::cerr << "Gamma=" << Gamma << std::endl;
                compute_full_step();
                
                double alpha = 1;
                for(size_t i=N;i>0;--i)
                {
                    pEq = peqs[i];
                    KEq = K[i];
                    const double G0 = Gamma[i];
                    std::cerr << "G0[" << i << "]=" << Gamma[i] << "/" << callG(0) << std::endl;
                    double       G1 = callG(alpha);
                    std::cerr << "G1=" << G1 << std::endl;
                }
                
                
                exit(0);
                break;
            }

            
        }

        


    }

}

