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
                const double Cj = (Ctry[j]=Cini[j]+alpha*dC[j]);
                if(active[j]&&(Cj<0))
                {
                    Ctry[j] = 0;
                }
            }
            updateGamma(Ctry);
            return GammaToScalar();
        }

#if 0
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
#endif
        
        
        void equilibria:: compute_full_step()
        {
            tao::mmul_rtrn(W,Phi,Nu);
            if(!LU<double>::build(W))
            {
                throw exception("%ssingular set of concentrations",fn);
            }
            tao::neg(xi,Gamma);
            LU<double>::solve(W,xi);
            tao::mul(dC,NuT,xi);
        }


        bool equilibria:: normalize(array<double> &C0, const double t) throw()
        {
            //const double threshold = numeric<double>::ftol;

            for(size_t j=M;j>0;--j)
            {
                Cini[j] = C0[j];
            }

            if(!balance(Cini))
            {
                std::cerr << "normalize: couldn't balance initial concentrations" << std::endl;
                return false;
            }

            // initialize K, Gamma, Phi @Cini
            initializeGammaAndPhi(Cini,t);
            double Gamma0 = GammaToScalar();
            size_t cycle  = 0;
            while(true)
            {
                ++cycle;
                //______________________________________________________________
                //
                // at this point, Cini is valid, Gamma and Phi are computed@Cini
                // => compute the full Newton's step
                //______________________________________________________________
                std::cerr << "Cini=" << Cini << "; Gamma0=" << Gamma0 << std::endl;
                tao::mmul_rtrn(W,Phi,Nu);
                if(!LU<double>::build(W))
                {
                    std::cerr << "normalize: singular system" << std::endl;
                    return false;
                }
                tao::neg(xi,Gamma);
                LU<double>::solve(W,xi);
                tao::mul(dC,NuT,xi);

                //______________________________________________________________
                //
                // this is Newton's predicted concentration
                //______________________________________________________________
                tao::setsum(C,Cini,dC);

                //______________________________________________________________
                //
                // which must be balanced
                //______________________________________________________________
                if(!balance(C))
                {
                    std::cerr << "normalize: couldn't balance internal concentrations" << std::endl;
                    return false;
                }
                tao::setvec(dC,C,Cini);
                updateGamma(C);
                double Gamma1 = GammaToScalar();
                std::cerr << "Gamma: " << Gamma0 << " -> " << Gamma1 << std::endl;
                if(Gamma1<=Gamma0)
                {
                    // check convergence

                    // ready for next step
                    tao::set(Cini,C);
                    updatePhi(Cini);
                    Gamma0 = Gamma1;
                    if(cycle>=100)
                    {
                        std::cerr << "exit@cycle=" << cycle << std::endl;
                        exit(0);
                    }
                }
                else
                {
                    std::cerr << "backtrack!" << std::endl;

                }

            }

            return false;

#if 0
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
#endif
        }

        


    }

}

