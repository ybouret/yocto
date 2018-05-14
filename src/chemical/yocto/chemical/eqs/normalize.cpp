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

        static inline
        void __optimize(math::numeric<double>::function &F,
                        triplet<double>                 &XX,
                        triplet<double>                 &FF) throw()
        {

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
            tao::mmul_rtrn(W,Phi,Nu);
            if(!LU<double>::build(W))
            {
                throw exception("%ssingular set of concentrations",fn);
            }
            tao::neg(xi,Gamma);
            LU<double>::solve(W,xi);
            tao::mul(dC,NuT,xi);
        }


        bool equilibria:: normalize(array<double> &C0,
                                    const double   t) throw()
        {
            const double threshold = numeric<double>::ftol;

            for(size_t j=M;j>0;--j)
            {
                Cini[j] = C0[j];
            }

            if(!balance(Cini))
            {
                return false;
            }

            //__________________________________________________________________
            //
            // initialize K, Gamma, Phi @Cini
            //__________________________________________________________________
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
                tao::mmul_rtrn(W,Phi,Nu);
                if(!LU<double>::build(W))
                {
                    return false;
                }
                tao::neg(xi,Gamma);
                LU<double>::solve(W,xi);
                tao::mul(dC,NuT,xi);

                //______________________________________________________________
                //
                // this is Newton's predicted concentration in Cend
                //______________________________________________________________
                tao::setsum(Cend,Cini,dC);

                //______________________________________________________________
                //
                // which must be balanced
                //______________________________________________________________
                if(!balance(Cend))
                {
                    // unable to balance...
                    return false;
                }

                //______________________________________________________________
                //
                // compute the effective dC, and update Gamma@Cend
                //______________________________________________________________
                tao::setvec(dC,Cini,Cend);
                updateGamma(Cend);

                //______________________________________________________________
                //
                // must be a total decreasing step in direction dC
                //______________________________________________________________
                double Gamma1 = GammaToScalar();
                
                //______________________________________________________________
                //
                // at this point,
                // a balanced C, and Gamma1=|Gamma|@C are computed
                //______________________________________________________________
                if(Gamma1>Gamma0)
                {
                    triplet<double> aa = { 0,      1,      1      };
                    triplet<double> gg = { Gamma0, Gamma1, Gamma1 };
                    bracket<double>::expand(normGamma,aa,gg);
                    aa.co_sort(gg);
                    __optimize(normGamma,aa,gg);
                    Gamma1 = __normGamma(max_of<double>(aa.b,0.0));

                    //__________________________________________________________
                    //
                    // retrieve C and dC from trial
                    // Gamma is updated@Ctry
                    //__________________________________________________________
                    for(size_t j=M;j>0;--j)
                    {
                        Cend[j]= Ctry[j];
                        dC[j]  = Cend[j]-Cini[j];
                    }
                }
                assert(Gamma1<=Gamma0);

                //______________________________________________________________
                //
                // at this point,
                // a balanced Cend, and Gamma1=|Gamma|@Cend are computed
                // with Gamma1<=Gamma0
                // and updated dC.
                // check convergence while updating Cini=Cend
                //______________________________________________________________
                bool converged = true;
                for(size_t j=M;j>0;--j)
                {
                    const double c_old = Cini[j];
                    const double c_new = Cend[j];
                    const double delta = Fabs(dC[j]);
                    if( (delta+delta) > threshold*( Fabs(c_old) + Fabs(c_new) ) )
                    {
                        converged = false;
                    }
                    Cini[j] = Cend[j];
                }
                if(converged)
                {
                    //std::cerr << "converged" << std::endl;
                    tao::set(C0,Cini,M);
                    return true;
                }

                //______________________________________________________________
                //
                // prepare for another loop
                //______________________________________________________________
                updatePhi(Cini);
                Gamma0=Gamma1;
            }

            return false;
        }

        


    }

}

