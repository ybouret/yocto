#include "yocto/alchemy/equilibria.hpp"
#include "yocto/code/utils.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/core/lu.hpp"

#include "yocto/math/opt/bracket.hpp"
#include "yocto/math/opt/minimize.hpp"

namespace yocto
{
    using namespace math;

    namespace alchemy
    {

        void equilibria:: computePhi(const array<double> &C0,
                                     const double         t)
        {
            size_t i = 1;
            for(iterator it=begin();i<=N;++i,++it)
            {
                const equilibrium &eq = **it;
                Gamma[i] = eq.computeGamma(C0, t, K[i]);
                eq.computeGradient(Phi[i], C0,    K[i]);
            }

        }

        void equilibria:: updatePhi(const array<double> &C0)
        {
            size_t i=1;
            for(iterator it=begin();i<=N;++i,++it)
            {
                const equilibrium &eq = **it;
                const double Kt = K[i];
                Gamma[i] = eq.updateGamma(C0,Kt);
                eq.computeGradient(Phi[i],C0,Kt);
            }

        }

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
            computePhi(C0,t);
            buildChi();
        }

        void equilibria:: updateChi(const array<double> &C0)
        {
            updatePhi(C0);
            buildChi();
        }

        void equilibria:: updateGamma(const array<double> &C0)
        {
            size_t i=1;
            for(iterator it=begin();i<=N;++i,++it)
            {
                const equilibrium &eq = **it;
                const double Kt = K[i];
                Gamma[i] = eq.updateGamma(C0,Kt);
            }
        }

        double equilibria:: call_F(const double alpha)
        {
            tao::setprobe(C, Cini, alpha, step);
            if(!balance())
            {
                throw exception("couldn't balance while normalizing, level-2");
            }
            updateGamma(C);
            return Gamma2Value();
        }

        void equilibria:: normalize(array<double> &C0, const double t)
        {
            static const char fn[] = "equilibria::normalize: ";
            //__________________________________________________________________
            //
            // initialize:
            // -- C <- C0
            // -- balancing
            // -- compute initial Chi, ie Gamma and Phi as well
            //__________________________________________________________________
            assert(C0.size()>=M);
            tao::set(C,C0);
            if(!balance())
            {
                throw exception("%scouldn't balance initial concentrations",fn);
            }

            computeChi(C,t);
            double  gam0 = Gamma2Value();

        LOOP:
            //__________________________________________________________________
            //
            // compute the raw extent from Newton's method
            //__________________________________________________________________
            tao::neg(xi,Gamma);
            LU<double>::solve(Chi,xi);
            tao::mul(step, NuT, xi);


            //__________________________________________________________________
            //
            // save starting point
            //__________________________________________________________________
            tao::set(Cini,C);


            //__________________________________________________________________
            //
            // try full step, optimistic way
            //__________________________________________________________________
            tao::add(C,step);
            if(!balance())
            {
                throw exception("%scouldn't balance while normalizing, level-1",fn);
            }
            updateChi(C);
            double gam1 = Gamma2Value();
            std::cerr << "gamma: " << gam1 << " <- " << gam0 << std::endl;


            if(gam1<gam0)
            {
                if(gam1<=0)
                {
                    //__________________________________________________________
                    //
                    // perfect numeric match, Phi and Chi are computed
                    //__________________________________________________________
                    std::cerr << "-- Perfect Newton's match" << std::endl;
                    goto DONE;
                }
                gam0 = gam1;
                goto LOOP;
            }
            else
            {
                std::cerr << "-- Optimizing" << std::endl;
                triplet<double> xx = { 0.0,  1.0,  -1.0 };
                triplet<double> FF = { gam0, gam1, -1.0 };
                bracket<double>::expand(F,xx,FF);
                optimize1D<double>::run(F,xx,FF,0.0);

                // take the best step and update C, Gamma, Phi and Chi
                const double alpha = max_of<double>(0.0,xx.b);
                tao::setprobe(C, Cini, alpha, step);
                updateChi(C);
                gam1 = Gamma2Value();
                if(gam1<gam0)
                {
                    if(gam1<=0)
                    {
                        //______________________________________________________
                        //
                        // optimized numeric match, Phi and Chi are computed
                        //______________________________________________________
                        std::cerr << "-- Optimized Newton's match" << std::endl;
                        goto DONE;
                    }
                    gam0 = gam1;
                    goto LOOP;
                }
                else
                {
                    std::cerr << "-- Numeric Stop" << std::endl;
                    //__________________________________________________________
                    //
                    // Numeric Stop!
                    //__________________________________________________________
                    goto DONE;
                }
            }


        DONE:
            for(size_t i=M;i>0;--i)
            {
                C0[i] = C[i];
            }
            //pLib->display(std::cerr,C0);


        }
        
        
    }
    
}

