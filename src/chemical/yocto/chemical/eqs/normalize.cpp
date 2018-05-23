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
                const double Cj = (Cend[j]=Cini[j]+alpha*step[j]);
                if(active[j])
                {
                    assert(Cini[j]>=0);
                    if(Cj<=0)
                    {
                        Cend[j] = 0;
                    }
                }
            }
            assert(is_valid(Cend));
            if(!balance(Cend))
            {
                throw;
            }
            updateGamma(Cend);
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
            //
            // prepare look up
            //
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
            //
            // initialize K, Gamma, Phi @Cini
            //
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
            updatePhi(Cini);


            //__________________________________________________________________
            //
            //
            // Newton's Loop
            //
            //__________________________________________________________________
        LOOP:
            {
                //______________________________________________________________
                //
                // Gamma, Gamma0, and Phi are computed at Cini
                // try to compute a step
                //______________________________________________________________
                assert(is_valid(Cini));
                if(!compute_step(Gamma0))
                {
                    // really bad system...
                    return false;
                }

                //______________________________________________________________
                //
                // adjusting step
                //______________________________________________________________
                double Gamma1 = 0;
            ADJUST_STEP:
                try
                {
                    Gamma1 = __NormGamma(1.0);
                }
                catch(...)
                {
                    std::cerr << "normalize.adjust" << std::endl;
                    //std::cerr << "@C=" << Cini << std::endl;
                    double rms = 0;
                    for(size_t j=M;j>0;--j)
                    {
                        const double as2 = square_of( step[j] *= 0.5 );
                        if( as2 <= numeric<double>::tiny )
                        {
                            step[j] = 0;
                        }
                        else
                        {
                            rms += as2;
                        }
                    }
                    if( (rms/M) <= 0 )
                    {
                        std::cerr  << "normalize:: unable to adjust step" << std::endl;
                        return false;
                    }
                    exit(0);
                    goto ADJUST_STEP;
                }

                //______________________________________________________________
                //
                // optimizing
                //______________________________________________________________
                //std::cerr << "step=" << step << std::endl;
                //std::cerr << "Gamma=" << Gamma << "; " << Gamma0 << " -> " << Gamma1 << std::endl;

                if(Gamma1<=0)
                {
                    //__________________________________________________________
                    //
                    // perfect
                    //__________________________________________________________
                    goto NORMALIZED;
                }
                else if(Gamma1<Gamma0)
                {
                    //__________________________________________________________
                    //
                    // accept
                    //__________________________________________________________
                    goto NEXT_STEP;
                }
                else
                {

                    //__________________________________________________________
                    //
                    // backtrack
                    //__________________________________________________________
                    assert(Gamma1>=Gamma0);
                    triplet<double> xx = { 0, 1, 1 };
                    triplet<double> ff = { Gamma0, Gamma1, Gamma1 };
                    try
                    {
                        bracket<double>::inside(NormGamma,xx,ff);
                        optimize1D<double>::run(NormGamma,xx,ff);
                        Gamma1 = __NormGamma(xx.b);
                    }
                    catch(...)
                    {
                        std::cerr << "normalized.bactracking failure" << std::endl;
                        return false;
                    }

                    if(Gamma1<=0)
                    {
                        goto NORMALIZED;
                    }
                    else
                    {
                        if(Gamma1>=Gamma0)
                        {
                            goto CHECK_EXTREMUM;
                        }
                        else
                        {
                            goto NEXT_STEP;
                        }
                    }

                }

            NEXT_STEP:
                //______________________________________________________________
                //
                // check convergence
                //______________________________________________________________
                {
                    bool   converged = true;
                    double rms       = 0;
                    for(size_t j=M;j>0;--j)
                    {
                        const double c_new = Cend[j];
                        if(active[j])
                        {
                            const double c_old = Cini[j];
                            const double delta = c_old-c_new;
                            if(converged)
                            {
                                const double c_try = c_old+delta;
                                if(Fabs(c_new-c_try)>0)
                                {
                                    converged = false;
                                }
                            }
                            const double d2 = delta*delta;
                            if(d2>numeric<double>::tiny)
                            {
                                rms += d2;
                            }
                        }
                        Cini[j] = c_new;
                    }
                    rms /= M;
                    if(converged||rms<=numeric<double>::tiny)
                    {
                        //std::cerr << "converged=" << converged << ", rms=" << rms << std::endl;
                        goto NORMALIZED;
                    }
                }

                //______________________________________________________________
                //
                // prepare for next step
                //______________________________________________________________
                Gamma0 = Gamma1;
                updatePhi(Cini);
                goto LOOP;

            CHECK_EXTREMUM:
                //std::cerr << "Extremum@" << Gamma1 << std::endl;
                ;

            NORMALIZED:
                //std::cerr << "Cend=" << Cend << "; Gamma1=" << Gamma1 << std::endl;
                tao::set(C0,Cend,M);

                return true;
            }


        }





    }

}

