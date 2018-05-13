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

        
        
        void equilibria:: compute_full_step()
        {
            //std::cerr << "Phi=" << Phi << std::endl;
            tao::mmul_rtrn(W,Phi,Nu);
            //std::cerr << "W=" << W << std::endl;
            if(!LU<double>::build(W))
            {
                throw exception("%ssingular set of concentrations",fn);
            }
            tao::neg(xi,Gamma);
            LU<double>::solve(W,xi);
            tao::mul(dC,NuT,xi);
            //std::cerr << "xi  =" << xi << std::endl;
            //std::cerr << "dC  =" << dC << std::endl;
        }


        void equilibria:: normalize(array<double> &C0, const double t)
        {
            const double threshold = numeric<double>::ftol;
            
            //__________________________________________________________________
            //
            // transfer data
            //__________________________________________________________________
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
                //std::cerr << "Cini =" << C     << std::endl;
                //std::cerr << "Gamma=" << Gamma << std::endl;
                compute_full_step();
                
                size_t nbad = 0;
                for(size_t j=M;j>0;--j)
                {
                    Ctry[j] = C[j]; // save
                    const double Cj =  (C[j]+=dC[j]);
                    if(active[j]&&Cj<0)
                    {
                        ++nbad;
                    }
                }
                if(nbad)
                {
                    std::cerr << "Bad!" << std::endl;
                    std::cerr << "C=" << C << std::endl;
                    if(!balance(C))
                    {
                        throw exception("%sunable to internally balance",fn);
                    }
                    else
                    {
                        std::cerr << "balanced..." << std::endl;
                        std::cerr << "C=" << C << std::endl;
                    }
                }
                bool converged = true;
                for(size_t j=M;j>0;--j)
                {
                    if(active[j])
                    {
                        const double c_new = C[j];
                        const double c_old = Ctry[j];
                        const double dC = Fabs(c_new-c_old);
                        const double CC = Fabs(c_new)+Fabs(c_old);
                        if(dC>threshold*CC)
                        {
                            converged=false;
                            break;
                        }
                    }
                }
                if(converged)
                {
                    std::cerr << "Converged" << std::endl;
                    break;
                }
                updateGammaAndPhi(C);
            }
            tao::set(C0,C,M);
            
        }

        


    }

}

