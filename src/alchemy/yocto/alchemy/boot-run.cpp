#include "yocto/alchemy/boot.hpp"
#include "yocto/math/core/lu.hpp"
#include "yocto/math/core/svd.hpp"
#include "yocto/math/core/tao.hpp"

#include "yocto/math/opt/bracket.hpp"
#include "yocto/math/opt/minimize.hpp"
#include "yocto/math/opt/cgrad.hpp"

#include "yocto/math/core/adjoint.hpp"


namespace yocto
{

    using namespace math;

    namespace alchemy
    {

        namespace
        {
            static const char fn[] = "boot.run: ";

            class BootContext
            {

            public:
                equilibria    &eqs;
                const library &lib;
                const size_t   N;
                const size_t   M;
                const size_t   Nc;
                matrix<double> P;
                vector<double> Lam;
                matrix<double> Q;
                matrix<double> PP;
                double         dPP;
                matrix<double> aPP;
                vector<double> Cstar;
                vector<double> Utemp;
                const double   t;
                array<double>  &C;
                vector<bool>    used;
                numeric<double>::scalar_field E;
                numeric<double>::vector_field G;
                vector<double>   Vcurr;
                vector<double>   dV;
                vector<double>   Vtemp;
                matrix<double>  &PhiQ;
                numeric<double>::function gam;

                inline BootContext(equilibria    &usrEqs,
                                   const library &usrLib,
                                   const size_t   usrNc,
                                   const double   usrT
                                   ) :
                eqs(usrEqs),
                lib(usrLib),
                N(eqs.size()),
                M(lib.size()),
                Nc(usrNc),
                P(Nc,M),
                Lam(Nc),
                Q(N,M),
                PP(Nc,Nc),
                dPP(0),
                aPP(Nc,Nc),
                Cstar(M),
                Utemp(Nc),
                t(usrT),
                C(eqs.C),
                used(N,true),
                E(this, & BootContext::call_E),
                G(this, & BootContext::call_G),
                Vcurr(N),
                dV(N),
                Vtemp(N),
                PhiQ(eqs.Chi),
                gam(this,& BootContext::call_F)
                {

                }

                void initialize()
                {
                    tao::mmul_rtrn(PP,P,P);
                    std::cerr << "PP=" << PP << std::endl;
                    const double dPP = ideterminant(PP);
                    if(Fabs(dPP)<0) throw exception("%singular constraints",fn);
                    matrix<double> aPP(Nc,Nc);
                    iadjoint(aPP,PP);


                    //__________________________________________________________
                    //
                    // compute orthogonal space
                    //__________________________________________________________
                    if(!svd<double>::orthonormal(Q,P))
                    {
                        throw exception("%sunable to build orthonormal space",fn);
                    }
                    std::cerr << "Q=" << Q << std::endl;

                    //__________________________________________________________
                    //
                    // compute Cstar
                    //__________________________________________________________
                    tao::mul(Utemp,aPP,Lam);
                    tao::mul_trn(Cstar,P,Utemp);
                    tao::divby(dPP,Cstar);
                    std::cerr << "Cstar=" << Cstar << std::endl;
                    std::cerr << "V0   =" << Vcurr  << std::endl;

                    //__________________________________________________________
                    //
                    // initialize Vcurr
                    //__________________________________________________________
                    moveV(Vcurr);
                    std::cerr << "V1=" << Vcurr << std::endl;
                    std::cerr << "C1=" << C     << std::endl;

                    //__________________________________________________________
                    //
                    // initialize Vcurr
                    //__________________________________________________________

                    eqs.computePhi(C,t);

                }

                void process()
                {
                    double gam0 = eqs.Gamma2Value();

                LOOP:
                    //__________________________________________________________
                    //
                    // compute the full Newton's step
                    // dV = -inv(Phi*Q') * Gamma;
                    //__________________________________________________________
                    tao::mmul_rtrn(PhiQ,eqs.Phi,Q);
                    if(!LU<double>::build(PhiQ))
                    {
                        throw exception("%ssingular composition",fn);
                    }
                    tao::neg(dV,eqs.Gamma);
                    LU<double>::solve(PhiQ,dV);

                    //__________________________________________________________
                    //
                    // optimize to be sure to decrease
                    //__________________________________________________________
                    std::cerr << "gam(0)=" << gam(0) << "/" << gam0 << std::endl;

                    triplet<double> xx = { 0, 1.0, -1.0 };
                    triplet<double> gg = { gam0, gam(xx.b), -1.0 }; std::cerr << "xx=" << xx << ", gg=" << gg << std::endl;
                    bracket<double>::expand(gam,xx,gg);             std::cerr << "xx=" << xx << ", gg=" << gg << std::endl;
                    optimize1D<double>::run(gam,xx,gg,0.0);         std::cerr << "xx=" << xx << ", gg=" << gg << std::endl;

                    const double alpha = max_of<double>(xx.b,0.0);
                    const double gam1  = gam(alpha);
                    std::cerr << "gam1=" << gam1 << "  <-- " << gam0 << std::endl;

                    if(gam1<gam0)
                    {
                        tao::set(Vcurr,Vtemp);
                        gam0 = gam1;
                        eqs.updatePhi(C);
                        goto LOOP;
                    }

                    //__________________________________________________________
                    //
                    // last good value
                    //__________________________________________________________
                    gen_C(Vcurr);
                    std::cerr << "C=" << C << std::endl;
                    svd<double>::truncate(C);
                    std::cerr << "C=" << C << std::endl;
                    eqs.updateGamma(C);
                    std::cerr << "Gamma=" << eqs.Gamma << std::endl;

                    tao::set(Utemp,Lam);
                    tao::mul_sub(Utemp, P,C);
                    const double rms = tao::RMS(Utemp);
                    std::cerr << "Utemp=" << Utemp << std::endl;
                    std::cerr << "rms="   << rms << std::endl;

                }


                inline void moveV(array<double> &V)
                {
                    (void)cgrad<double>::optimize(E,
                                                  G,
                                                  V,
                                                  used,
                                                  0.0);
                    gen_C(V);
                }

                inline ~BootContext() throw()
                {
                }

                inline void gen_C(const array<double> &V) throw()
                {
                    tao::set(C,Cstar);
                    tao::mul_add_trn(C,Q,V);
                }

                inline double call_E(const array<double> &V) throw()
                {
                    gen_C(V);
                    double ans = 0.0;
                    for(size_t i=C.size();i>0;--i)
                    {
                        if(eqs.active[i])
                        {
                            const double Ci = C[i];
                            if(Ci<0)
                            {
                                ans += Ci*Ci;
                            }
                        }
                    }
                    return 0.5*ans;
                }

                inline void call_G(array<double>       &G,
                                   const array<double> &V) throw()
                {
                    gen_C(V);
                    for(size_t i=C.size();i>0;--i)
                    {
                        eqs.beta[i] = 0;
                        if(eqs.active[i])
                        {
                            const double Ci = C[i];
                            if(Ci<0)
                            {
                                eqs.beta[i] = Ci;
                            }
                        }
                    }
                    tao::mul(G,Q,eqs.beta);
                }

                inline double call_F(double alpha)
                {
                    tao::setprobe(Vtemp, Vcurr, alpha, dV);
                    moveV(Vtemp);
                    eqs.updateGamma(C);
                    return eqs.Gamma2Value();
                }

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(BootContext);
            };
        }


        void boot:: run( equilibria &eqs, const double t )
        {

            //__________________________________________________________________
            //
            // check parameters
            //__________________________________________________________________

            const size_t Nc = constraints.size();
            const size_t N  = eqs.size();
            const size_t M  = pLib->size();
            if(M!=N+Nc)
            {
                throw exception("%s#species=%u != #equations=%u + #constraints=%u",
                                fn,
                                unsigned(M),
                                unsigned(N),
                                unsigned(Nc)
                                );
            }

            //__________________________________________________________________
            //
            // build the constraint matrix
            //__________________________________________________________________
            if(Nc<=0)
            {
                // no constraints

                return;
            }

            if(Nc>=M)
            {
                // no reactions

                return;
            }

            //__________________________________________________________________
            //
            // build the constraint matrix
            //__________________________________________________________________
            BootContext ctx(eqs,*pLib,Nc,t);

            for(size_t i=1;i<=Nc;++i)
            {
                const constraint &cc = *constraints[i];
                ctx.Lam[i] = cc.value;
                for(component::database::const_iterator j = cc.begin(); j!= cc.end(); ++j)
                {
                    const component &comp = *j;
                    const size_t     k    = comp.sp->indx;
                    ctx.P[i][k] = comp.weight;
                }
            }
            std::cerr << "P="   << ctx.P   << std::endl;
            std::cerr << "Lam=" << ctx.Lam << std::endl;

            ctx.initialize();
            ctx.process();

        }



#if 0
        void boot:: run( equilibria &eqs, const double t )
        {
            static const char fn[] = "boot.run: ";
            // check parameters
            const size_t Nc = constraints.size();
            const size_t N  = eqs.size();
            const size_t M  = pLib->size();
            if(M!=N+Nc)
            {
                throw exception("#species=%u != #equations=%u + #constraints=%u",
                                unsigned(M),
                                unsigned(N),
                                unsigned(Nc)
                                );
            }

            array<double>  &C     = eqs.C;
            array<double>  &dC    = eqs.dC;
            array<double>  &Gamma = eqs.Gamma;
            matrix<double> &Phi   = eqs.Phi;

            assert(C.size()==M);

            matrix<double> P;
            vector<double> Lam;

            //__________________________________________________________________
            //
            // build the constraint matrix
            //__________________________________________________________________

            if(Nc<=0)
            {
                // no constraints

                return;
            }

            if(Nc>=M)
            {
                // no reactions

                return;
            }

            start_C.make(M);
            delta_C.make(M);
            pEqs = &eqs;

            P.  make(Nc,M);
            Lam.make(Nc);
            for(size_t i=1;i<=Nc;++i)
            {
                const constraint &cc = *constraints[i];
                Lam[i] = cc.value;
                for(component::database::const_iterator j = cc.begin(); j!= cc.end(); ++j)
                {
                    const component &comp = *j;
                    const size_t     k    = comp.sp->indx;
                    P[i][k] = comp.weight;
                }
            }
            std::cerr << "P="   << P   << std::endl;
            std::cerr << "Lam=" << Lam << std::endl;

            //__________________________________________________________________
            //
            // check the rank and inv(P*P') as adjoint and determinant
            //__________________________________________________________________
            matrix<double> P2(Nc,Nc);
            tao::mmul_rtrn(P2,P,P);
            std::cerr << "P2=" << P2 << std::endl;
            const double dPP = ideterminant(P2);
            if(Fabs(dPP)<0) throw exception("%singular constraints",fn);
            matrix<double> aPP(Nc,Nc);
            iadjoint(aPP,P2);


            //__________________________________________________________________
            //
            // compute orthogonal space
            //__________________________________________________________________
            matrix<double> Q(N,M);
            if(!svd<double>::orthonormal(Q,P))
            {
                throw exception("%sunable to build orthonormal space",fn);
            }
            std::cerr << "Q=" << Q << std::endl;


            //__________________________________________________________________
            //
            // compute Cstar
            //__________________________________________________________________
            vector<double> Cstar(M);
            vector<double> Utmp(Lam);
            tao::mul(Utmp,aPP,Lam);

            tao::mul_trn(Cstar,P,Utmp);
            tao::divby(dPP,Cstar);
            std::cerr << "Cstar=" << Cstar << std::endl;


            //__________________________________________________________________
            //
            // prepare memory
            //__________________________________________________________________
            vector<double> dV(N);
            vector<double> dU(Nc);

            matrix<double> PhiQ(N);
            numeric<double>::function F(this, & boot::call_F );


            //__________________________________________________________________
            //
            // initialize C to a validated Cstar and V
            //__________________________________________________________________
            tao::set(C,Cstar);
            eqs.validate(C);
            eqs.computePhi(C,t);
            double F0 = eqs.Gamma2Value();

        LOOP:

            //__________________________________________________________________
            //
            // assuming Gamma,Phi and F0
            //
            // compute dU = inv(P*P')*(Lam-P*C)
            //__________________________________________________________________
            tao::set(Utmp,Lam);                // Utmp = Lambda
            tao::mul_sub(Utmp,P,C);            // Utmp = Lambda-P*C
            tao::mul_and_div(dU,aPP,Utmp,dPP); // dU = inv(P*P')*(Lam-P*C)
            tao::mul_trn(dC,P,dU);             // dC = P'*U

            //__________________________________________________________________
            //
            // compute dV = -inv(Phi*Q')*(Gamma+Phi*dC)
            //__________________________________________________________________
            tao::mmul_rtrn(PhiQ,Phi,Q);
            if(!LU<double>::build(PhiQ))
            {
                throw exception("%sinvalid Jacobian",fn);
            }
            tao::set(dV,Gamma);
            tao::mul_add(dV,Phi,dC);
            tao::neg(dV,dV);
            LU<double>::solve(PhiQ,dV);


            //__________________________________________________________________
            //
            // save start_C and compute delta_C
            //__________________________________________________________________
            tao::set(start_C,C);
            tao::set(delta_C,dC);
            tao::mul_add_trn(delta_C,Q,dV);

            //__________________________________________________________________
            //
            // find a global minimum of the non linear part, avoiding
            // oscillations
            //__________________________________________________________________
            triplet<double> xx = { 0.0, 1.0,     -1 };
            triplet<double> ff = { F0,  F(xx.b), -1 };
            bracket<double>::expand(F,xx,ff);
            optimize1D<double>::run(F,xx,ff,0.0);

            //const double alpha = (xx.b < numeric<double>::sqrt_ftol) ? 0.0 : xx.b;
            const double alpha = max_of<double>(xx.b,0);
            const double F1    = F(alpha);
            std::cerr << "F1=" << F1 << " <-- " << F0 << ", alpha=" << alpha << " / ftol=" << numeric<double>::ftol << " / sqrt_ftol=" << numeric<double>::sqrt_ftol << std::endl;
            std::cerr << "C="  << C  << std::endl;

            exit(0);

            if(F1<F0)
            {
                std::cerr << "F0-F1=" << F0-F1 << std::endl;
                std::cerr << "F0+F1=" << F0+F1 << std::endl;
                std::cerr << "(F0-F1)/(F0+F1)=" << (F0-F1)/(F0+F1) << std::endl;
            }


            if(F1<F0)
            {
                //______________________________________________________________
                //
                // decreasing
                //______________________________________________________________
                std::cerr << "decreasing..." << std::endl;
                eqs.updatePhi(C);
                F0 =  F1;
                goto LOOP;
            }
            else
            {
                //______________________________________________________________
                //
                // not decreasing, save last C and break
                //______________________________________________________________
                tao::set(C,start_C);
            }

            //__________________________________________________________________
            //
            // check validity: non linear part
            //__________________________________________________________________
            eqs.updateGamma(C);
            const double F_end = eqs.Gamma2Value();
            if(F_end>0)
            {
                svd<double>::truncate(C);
            }

            //__________________________________________________________________
            //
            // check validity: linear part
            //__________________________________________________________________
            tao::mul(Utmp,P,C);
            tao::sub(Utmp,Lam);
            const double rms = tao::RMS(Utmp);
            if(rms>numeric<double>::ftol)
            {
                throw exception("%s:numerical failure for constraints", fn);
            }




        }

        double boot:: call_F(double alpha)
        {
            assert(pEqs);
            // create a probe
            tao::setprobe(pEqs->C, start_C, alpha, delta_C);

            // clamp the probe
            //pEqs->validate(pEqs->C);

            // compute gamma
            pEqs->updateGamma(pEqs->C);

            // return objective value
            return pEqs->Gamma2Value();
        }

#endif

#if 0
        class PositiveFinder
        {
        public:
            const matrix<double> &Q;
            const array<double>  &Cstar;
            array<double>        &C;
            const array<bool>    &active;
            const array<bool>    &used;
            array<double>        &beta;

            math::numeric<double>::scalar_field E;
            math::numeric<double>::vector_field G;

            inline PositiveFinder(const matrix<double> &usrQ,
                                  const array<double>  &usrCstar,
                                  array<double>        &usrC,
                                  const array<bool>    &usrActive,
                                  const array<bool>    &usrUsed,
                                  array<double>        &usrBeta) throw() :
            Q(usrQ),
            Cstar(usrCstar),
            C(usrC),
            active(usrActive),
            used(usrUsed),
            beta(usrBeta),
            E(this, & PositiveFinder::call_E),
            G(this, & PositiveFinder::call_G)
            {
                assert(Cstar.size()==C.size());
                assert(C.size()==active.size());
            }

            inline ~PositiveFinder() throw() {}

            inline void move(array<double> &V)
            {
                (void) cgrad<double>::optimize(E,G,V,used,0.0,NULL);
            }

            inline void gen_C(const array<double> &V) throw()
            {
                tao::set(C,Cstar);
                tao::mul_add_trn(C,Q,V);
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(PositiveFinder);

            inline double call_E(const array<double> &V) throw()
            {
                gen_C(V);
                double ans = 0.0;
                for(size_t i=C.size();i>0;--i)
                {
                    if(active[i])
                    {
                        const double Ci = C[i];
                        if(Ci<0)
                        {
                            ans += Ci*Ci;
                        }
                    }
                }
                return 0.5*ans;
            }

            inline void call_G(array<double>       &G,
                               const array<double> &V) throw()
            {
                gen_C(V);
                for(size_t i=C.size();i>0;--i)
                {
                    beta[i] = 0;
                    if(active[i])
                    {
                        const double Ci = C[i];
                        if(Ci<0)
                        {
                            beta[i] = Ci;
                        }
                    }
                }
                tao::mul(G,Q,beta);
            }
        };

        class BootOptimizer
        {
        public:
            PositiveFinder            &pfn;
            array<double>             &V;
            vector<double>             Vstart;
            vector<double>             Vdelta;
            equilibria                &eqs;
            numeric<double>::function  F;
            
            inline BootOptimizer(PositiveFinder      &usrPFN,
                                 array<double>       &usrV,
                                 equilibria          &usrEqs) :
            pfn(usrPFN),
            V( usrV ),
            Vstart( V.size() ),
            Vdelta( V.size() ),
            eqs(usrEqs),
            F(this, & BootOptimizer::call_F )
            {
            }
            
            inline ~BootOptimizer() throw()
            {
            }
            
            double call_F(const double alpha)
            {
                tao::setprobe(V,Vstart,alpha,Vdelta);
                pfn.move(V);
                pfn.gen_C(V);
                eqs.updateGamma(pfn.C);
                return eqs.Gamma2Value();
            }
            
            
            
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(BootOptimizer);
        };
#endif
        
        
        
    }
    
}
