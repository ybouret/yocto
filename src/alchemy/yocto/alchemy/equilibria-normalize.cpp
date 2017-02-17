#include "yocto/alchemy/equilibria.hpp"
#include "yocto/code/utils.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/core/lu.hpp"

namespace yocto
{
    using namespace math;
    
    namespace alchemy
    {
        void equilibria:: buildChi()
        {
            tao::mmul_rtrn(Chi, Phi, Nu);
            if(!LU<double>::build(Chi))
            {
                throw exception("equilibria: singular composition");
            }
        }

        void equilibria:: computeChi(const array<double> &C0,
                                     const double         t)
        {
            size_t i = 1;
            for(iterator it=begin();i<=N;++i,++it)
            {
                const equilibrium &eq = **it;
                Gamma[i] = eq.computeGamma(C0, t, K[i]);
                eq.computeGradient(Phi[i], C0,    K[i]);
            }
            buildChi();
        }

        void equilibria:: updateChi(const array<double> &C0)
        {
            size_t i=1;
            for(iterator it=begin();i<=N;++i,++it)
            {
                const equilibrium &eq = **it;
                const double Kt = K[i];
                Gamma[i] = eq.updateGamma(C0,Kt);
                eq.computeGradient(Phi[i],C0,Kt);
            }
            buildChi();
        }


        void equilibria:: normalize(array<double> &C0, const double t)
        {
            static const char fn[] = "equilibria::boot: ";
            //__________________________________________________________________
            //
            // initialize
            //__________________________________________________________________
            assert(C0.size()>=M);
            tao::set(C,C0);
            if(!balance()) throw exception("%scouldn't balance initial concentrations",fn);


            computeChi(C,t);

            double norm2=0;
            bool   check=false;

            while(true)
            {
                //______________________________________________________________
                //
                // compute the raw extent
                //______________________________________________________________
                std::cerr << std::endl;
                std::cerr << "Gamma=" << Gamma << std::endl;
                tao::neg(xi,Gamma);
                LU<double>::solve(Chi,xi);

                std::cerr << "C  =" << C  << std::endl;
                std::cerr << "xi =" << xi << std::endl;



                tao::mul(dC, NuT, xi);
                tao::add(C, dC);
                if(!balance()) throw exception("%scouldn't balance current concentrations",fn);

                const double temp2 = tao::norm_sq(dC);
                if(check)
                {
                    std::cerr << "norm=" << temp2 << "/" << norm2 << std::endl;
                    if(temp2>=norm2)
                    {
                        std::cerr << "Reached equilibrium" << std::endl;
                        // set back value
                        for(size_t i=M;i>0;--i)
                        {
                            C0[i] = C[i];
                        }
                        break;
                    }
                    norm2 = temp2;
                }
                else
                {
                    check = true;
                    norm2 = temp2;
                }
                updateChi(C);
            }
            
        }


    }

}

