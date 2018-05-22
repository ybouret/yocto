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

        double equilibria::__GammaEq(double alpha)
        {
            assert(pEq);
            for(size_t j=M;j>0;--j)
            {
                const double Cj = (Ctry[j]=Cini[j]+alpha*dC[j]);
                if(active[j]&&(Cj<0))
                {
                    Ctry[j] = 0;
                }
            }
            return pEq->computeGamma(Ctry,KEq);
        }


        void equilibria:: move( const size_t iEq ) throw()
        {
            assert(iEq>=1);
            assert(iEq<=N);
            
            KEq = K[iEq];
            pEq = peqs[iEq];
            tao::set(dC,Nu[iEq]);

            std::cerr << "move: " << pEq->name << " (K=" << KEq << ")  from  " << Cini << std::endl;
            triplet<double>    x = { 0.0, 0.0, 0.0 };
            triplet<double>    G = { __GammaEq(x.a), 0, 0 };
            const int          s = sign_of(G.a);
            equilibrium::range r;
            std::cerr << "Gam=" << G.a << std::endl;
            switch(s)
            {
                case  0: std::cerr << "no" << std::endl; return;
                case -1: pEq->compute_reverse(r,Cini); break;
                case  1: pEq->compute_forward(r,Cini); break;
                default:
                    // never get here
                    return;
            }
            std::cerr << "range: " << r << std::endl;
            if(r.exists)
            {
                std::cerr << "using " << r.xi << std::endl;
                x.c = r.xi;
                G.c = __GammaEq(x.c);
            }
            else
            {
                std::cerr << "looking for extent" << std::endl;
                x.c = s; // TODO: better choice ?
                while( (G.c=__GammaEq(x.c)) * G.a > 0 )
                {
                    ++x.c;
                }
            }
            std::cerr << "x=" << x << std::endl;
            std::cerr << "G=" << G << std::endl;
            const double alpha = zsolve.run(GammaEq,x,G);
            std::cerr << "alpha=" << alpha << std::endl;

            for(size_t j=M;j>0;--j)
            {
                Cini[j] = Cini[j] + alpha * dC[j];
                if(active[j]&&Cini[j]<0)
                {
                    Cini[j]=0;
                }
            }
            std::cerr << "=> C=" << Cini << std::endl;
        }


        void equilibria:: sweep() throw()
        {
            for(size_t i=N;i>0;--i)
            {
                move(i);
            }
        }


        double equilibria:: __NormGamma(double alpha)
        {
            for(size_t j=M;j>0;--j)
            {
                const double Cj = (Ctry[j]=Cini[j]+alpha*step[j]);
                if(active[j])
                {
                    assert(Cini[j]>=0);
                    if(Cj<=0)
                    {
                        Ctry[j] = 0;
                    }
                }
            }
            assert(is_valid(Ctry));
            updateGamma(Ctry);
            return GammaToScalar();
        }

        bool equilibria:: is_valid(const array<double> &C0) const throw()
        {
            for(size_t j=M;j>0;--j)
            {
                if(active[j]&&C0[j]<0) return false;
            }
            return true;
        }

        bool equilibria:: compute_step(double &GammaIni) throw()
        {
            //__________________________________________________________________
            //
            //
            // from a balanced concentration Cini, with Gamma and Phi computed
            //
            //__________________________________________________________________
            tao::mmul_rtrn(W,Phi,Nu);
            if(!LU<double>::build(W))
            {
                //______________________________________________________________
                //
                // OK, let's try to sweep a little bit off position
                //______________________________________________________________
                sweep();
                updateGammaAndPhi(Cini);
                GammaIni = GammaToScalar();
                tao::mmul_rtrn(W,Phi,Nu);
                if(!LU<double>::build(W))
                {
                    return false; // this was the last best effort...
                }
            }
            tao::neg(xi,Gamma);
            LU<double>::solve(W,xi);
            tao::mul(step,NuT,xi);
            return true;
        }


        bool equilibria:: normalize(array<double> &C0,
                                    const double   t,
                                    const bool     initialize) throw()
        {

            //__________________________________________________________________
            //
            // prepare look up
            //__________________________________________________________________

            for(size_t j=M;j>0;--j)
            {
                Cini[j] = C0[j];
            }

            if(!balance(Cini))
            {
                return false;
            }
            assert(is_valid(Cini));

            //__________________________________________________________________
            //
            // initialize K, Gamma, Phi @Cini
            //__________________________________________________________________
            if(initialize)
            {

                // compute all K and Gamma
                initializeGamma(Cini,t);
            }
            else
            {
                // assuming K are already computed
                updateGamma(Cini);
            }

            double Gamma0 = GammaToScalar();
            if(Gamma0<=0)
            {
                return true;
            }
            // monitor jumps height!
            updatePhi(Cini);
            size_t jumps    = 0;
            double GammaTop = Gamma0;
            while(true)
            {
                //______________________________________________________________
                //
                // Gamma, Gamma0, and Phi are computed at Cini
                // Try to compute a step and final concentration
                //______________________________________________________________
                assert(is_valid(Cini));
                if(!compute_step(Gamma0))
                {
                    // really bad system...
                    return false;
                }
                tao::setsum(Cend,Cini,step);
                bool changed = false;
                if(!balance(Cend,&changed))
                {
                    std::cerr << "couldn't balance Cend=" << Cend << std::endl;
                    return false;
                }

                //______________________________________________________________
                //
                //
                // compute the effective step, which is in NuT space...
                //
                //______________________________________________________________
                assert(is_valid(Cini));
                assert(is_valid(Cend));
                updateGamma(Cend);
                double Gamma1 = GammaToScalar();
                std::cerr << "Gamma: " << Gamma0 << "->" << Gamma1 << std::endl;
                if(Gamma1<=0)
                {
                    //__________________________________________________________
                    //
                    // perfect !
                    //__________________________________________________________
                    goto NORMALIZED;
                }
                else if(Gamma1<Gamma0)
                {
                    //__________________________________________________________
                    //
                    // good!
                    //__________________________________________________________
                    goto CONTINUE;
                }
                else
                {
                    assert(Gamma1>=Gamma0);
                    if(changed)
                    {
                        // has changed=>accept new position?
                        ++jumps;
                        std::cerr << "jump#" << jumps << std::endl;
                        std::cerr << "step=" << step << std::endl;
                        std::cerr << "Cini=" << Cini << std::endl;
                        std::cerr << "Cend=" << Cend << std::endl;

                        double alpha =1;
                        while(true)
                        {
                            alpha /= 2;
                            if(alpha<=0)
                            {
                                std::cerr << "unable to move forward..." << std::endl;
                                return false;
                            }
                            tao::setprobe(Cend, Cini, alpha, step);
                            if(!balance(Cend))
                            {
                                continue;
                            }
                            updateGamma(Cend);
                            Gamma1 = GammaToScalar();
                            std::cerr << "\t->Gamma1=" << Gamma1 << std::endl;
                            if(Gamma1<GammaTop)
                            {
                                GammaTop = Gamma1;
                                break;
                            }
                        }
                        goto CONTINUE;
                    }
                    else
                    {
                        //______________________________________________________
                        //
                        // jsut backtrack inside
                        //______________________________________________________
                        tao::setvec(step,Cini,Cend); // recompute the full step
                        triplet<double> xx = {0,1,1};
                        triplet<double> ff = {Gamma0,Gamma1,Gamma1};
                        bracket<double>::inside(NormGamma,xx,ff);
                        optimize1D<double>::run(NormGamma,xx,ff);
                        Gamma1 = __NormGamma(xx.b);

                        if(Gamma1>=Gamma0)
                        {
                            //__________________________________________________
                            //
                            // must accept
                            //__________________________________________________
                            goto NORMALIZED;
                        }
                        else
                        {
                            //__________________________________________________
                            //
                            // continue;
                            //__________________________________________________
                            goto CONTINUE;
                        }
                    }
                }

            CONTINUE:
                Gamma0 = Gamma1;
                tao::set(Cini,Cend);
                updatePhi(Cini);

            }

        NORMALIZED:
            tao::set(C0,Cend,M);
            assert(is_valid(C0));
            return true;
        }


#if 0
        bool equilibria:: normalize(array<double> &C0,
                                    const double   t,
                                    const bool     initialize) throw()
        {
            const double ftol = numeric<double>::ftol;
            const double tiny = numeric<double>::tiny;
            
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
            if(initialize)
            {

                // compute all K and Gamma
                initializeGamma(Cini,t);
                //std::cerr << "C_in=" << Cini << std::endl;
            }
            else
            {
                // assuming K are already computed
                updateGamma(Cini);
                //std::cerr << "C_up=" << Cini << std::endl;
            }
            
            double Gamma0 = GammaToScalar();
            if(Gamma0<=0)
            {
                //std::cerr << "already normalized" << std::endl;
                return true;
            }
            updatePhi(Cini);
            size_t cycles  = 0;
            while(true)
            {

                ++cycles;
                //______________________________________________________________
                //
                // at this point, Cini is valid, Gamma and Phi are computed@Cini
                // => compute the full Newton's step
                //______________________________________________________________
                if(!compute_step())
                {
                    // really, really singular composition and system...
                    return false;
                }

                //______________________________________________________________
                //
                // this is Newton's predicted concentration in Cend
                //______________________________________________________________
                tao::setsum(Cend,Cini,dC);

                //______________________________________________________________
                //
                // which must be balanced
                //______________________________________________________________
                bool changed = false; // keep track if something was changed
                if(!balance(Cend,&changed))
                {
                    // unable to balance...
                    return false;
                }

                //if(changed) std::cerr << "changed!" << std::endl;

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
                //std::cerr << "Gamma=" << Gamma << std::endl;
                //std::cerr << "Gamma1=" << Gamma1 << "/" << Gamma0 << std::endl;

                //______________________________________________________________
                //
                // at this point,
                // a balanced Cend, and Gamma1=|Gamma|@Cend are computed
                //______________________________________________________________
                if(!changed)
                {
                    if(Gamma1>Gamma0)
                    {
                        triplet<double> aa = { 0,      1,      1      };
                        triplet<double> gg = { Gamma0, Gamma1, Gamma1 };
                        bracket<double>::inside(NormGamma,aa,gg);
                        optimize1D<double>::run(NormGamma,aa,gg);

                        Gamma1 = __NormGamma(max_of<double>(aa.b,0.0));

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
                        //std::cerr << "->Gamma1=" << Gamma1 << "/" << Gamma0 << std::endl;
                    }
                    assert(Gamma1<=Gamma0);
                }

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
                    if(active[j])
                    {
                        const double delta = Fabs(dC[j]);
                        if( (delta+delta) > ftol*( Fabs(c_old) + Fabs(c_new) ) )
                        {
                            converged = false;
                        }
                    }
                    Cini[j] = c_new;
                }

                //______________________________________________________________
                //
                // second level in case of numeric noise
                //______________________________________________________________
                if(!converged)
                {
                    //std::cerr << "Testing dC=" << dC << std::endl;
                    converged = true;
                    for(size_t j=M;j>0;--j)
                    {
                        if( square_of(dC[j]) > tiny )
                        {
                            converged=false;
                            break;
                        }
                    }

                }

                if(converged)
                {
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

        }
#endif
        


    }

}

