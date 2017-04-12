#include "yocto/alchemy/boot.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/core/svd.hpp"
#include "yocto/math/core/lu.hpp"

#include "yocto/ios/ocstream.hpp"
#include "yocto/math/opt/bracket.hpp"
#include "yocto/math/opt/minimize.hpp"




namespace yocto
{
    using namespace math;

    namespace alchemy
    {

        static const char fn[] = "boot::solution: ";
        typedef svd<double> SVD;

        class BootSol
        {
        public:
            equilibria    &eqs;
            const library &lib;
            const size_t   M;
            const size_t   N;
            const size_t   Nc;
            matrix<double> P;
            matrix<double> Q;
            vector<double> Lam;
            vector<double> C;
            vector<double> Cstar;
            vector<double> Ctemp;
            vector<double> V;
            vector<double> Vtemp;
            vector<double> dV;
            vector<double> beta;
            vector<double> eta;

            numeric<double>::function F;

            inline BootSol(boot                             &ran,
                           equilibria                       &usr_eqs,
                           const library                    &usr_lib,
                           array<boot::constraint::pointer> &cst,
                           const double                      t ) :
            eqs(usr_eqs),
            lib(usr_lib),
            M(lib.size()),
            N(eqs.size()),
            Nc(cst.size()),
            P(),
            Lam(),
            C(),
            Cstar(),
            Ctemp(),
            V(),
            Vtemp(),
            dV(),
            beta(),
            eta(),
            F( this, & BootSol :: call_F )
            {
                if(Nc+N!=M)
                    throw exception("%s#constraints=%u + #equilibria=%u != #species=%u", fn, unsigned(Nc), unsigned(N), unsigned(M) );

                //______________________________________________________________
                //
                // special cases
                //______________________________________________________________


                //______________________________________________________________
                //
                // generic case
                //______________________________________________________________
                P.make(Nc,M);
                Q.make(N,M);
                Lam.make(Nc);
                C.make(M);
                Cstar.make(M);
                Ctemp.make(M);
                V.make(N);
                Vtemp.make(N);
                dV.make(N);
                beta.make(M);
                eta.make(N);
                matrix<double> &Phi   = eqs.Phi;
                matrix<double> &PhiQ  = eqs.Chi;
                array<double>  &Gamma = eqs.Gamma;

                //______________________________________________________________
                //
                // create linear system
                //______________________________________________________________
                for(size_t i=1;i<=Nc;++i)
                {
                    const boot::constraint &cc = *cst[i];
                    Lam[i] = cc.value;
                    for( boot::constraint::const_iterator it = cc.begin(); it != cc.end(); ++it)
                    {
                        const boot::component &comp = *it;
                        const size_t j = comp.sp->indx;
                        P[i][j] = comp.weight;
                    }
                }
                std::cerr << "P  =" << P << std::endl;
                std::cerr << "Lam=" << Lam << std::endl;

                //______________________________________________________________
                //
                // compute Cstar = P'*inv(P*P')*Lam
                //______________________________________________________________
                {
                    matrix<double> P2(Nc);
                    tao::mmul_rtrn(P2,P,P);
                    std::cerr << "P2=" << P2 << std::endl;
                    if( !LU<double>::build(P2) )
                    {
                        throw exception("%ssingular set of constraints",fn);
                    }
                    vector<double> U(Nc);
                    tao::set(U,Lam);
                    LU<double>::solve(P2,U);
                    tao::mul_trn(Cstar,P,U);
                }
                std::cerr << "Cstar=" << Cstar << std::endl;

                //______________________________________________________________
                //
                // compute orthonormal matrix
                //______________________________________________________________
                if( !SVD::orthonormal(Q,P) )
                {
                    throw exception("%sunable to build orthonormal matrix!",fn);
                }
                SVD::truncate(Q);
                std::cerr << "Q=" << Q << std::endl;

                //______________________________________________________________
                //
                // compute concentration space, while loading constants
                // for subsequent computations
                //______________________________________________________________
                double Cmin = 0;
                double Cmax = 0;
                {
                    size_t i    = 0;
                    for(equilibria::iterator ii=eqs.begin();ii!=eqs.end();++ii)
                    {
                        const equilibrium &eq = **ii;

                        ++i;
                        const double Ki    = ( eqs.K[i] = eq.K(t) );
                        const double delta = eq.delta_nu();
                        if(delta)
                        {
                            const double Ci = pow(Ki,1.0/delta);
                            std::cerr << "\tC_{" << eq.name << "}=" << Ci << std::endl;
                            if(Cmin>0)
                            {
                                Cmin = min_of(Ci,Cmin);
                            }
                            else
                            {
                                Cmin = Ci;
                            }
                            Cmax = max_of(Ci,Cmax);
                        }
                    }
                }

                std::cerr << "\tCmin=" << Cmin << std::endl;
                std::cerr << "\tCmax=" << Cmax << std::endl;

                if(Cmax<=0)
                {
                    // 0 is solution...
                    return;
                }

                const double Camp = Cmax - Cmin;

//#define COMPUTE_H() (eqs.Gamma2Value())
#define COMPUTE_H() (0.5*tao::norm_sq(eqs.Gamma))

                //______________________________________________________________
                //
                //
                // Making a trial from a random configuration
                //
                //______________________________________________________________
                size_t num_trials=0;

            NEW_TRIAL:
                ++num_trials;

                //______________________________________________________________
                //
                // generate initial configuration
                //______________________________________________________________
                for(size_t j=M;j>0;--j)
                {
                    C[j] = Cmin + 100 * ran() * Camp;
                }
                std::cerr << "Cran=" << C << std::endl;

                //______________________________________________________________
                //
                // project on the legal space to get initial V
                //______________________________________________________________
                tao::mul(V,Q,C);
                gen_C(C,V);

                // initialiaze loop data
                eqs.updatePhi(C);
                double H0 = COMPUTE_H();

            LOOP:
                std::cerr << "Cini=" << C << std::endl;
                std::cerr << "Vini=" << V << std::endl;
                std::cerr << "\tH0=" << H0 << std::endl;

                // initialize Newton's step
                tao::mmul_rtrn(PhiQ, Phi, Q);
                if( ! LU<double>::build(PhiQ) )
                {
                    std::cerr << "Need to restart..." << std::endl;
                    goto NEW_TRIAL;
                }
                tao::neg(dV,Gamma);
                LU<double>::solve(PhiQ,dV);


                double H1 = F(1.0);
                std::cerr << "\tH1=" << H1 << std::endl;

                if(H1<H0)
                {
                    H0=H1;
                    tao::set(V,Vtemp);
                    tao::set(C,Ctemp);
                    eqs.updatePhi(C);
                    goto LOOP;
                }
                else
                {
                    std::cerr << "-- Optimize H" << std::endl;
                    triplet<double> xx = { 0.0, 0.5, -1 };
                    triplet<double> hh = { H0,  F(xx.b), -1 };
                    bracket<double>::expand(F,xx,hh);
                    std::cerr << "xx=" << xx << ", hh=" << hh << std::endl;
                    optimize1D<double>::run(F,xx,hh,0.0);
                    const double alpha = max_of(0.0,xx.b);
                    std::cerr << "alpha=" << alpha << std::endl;
                    H1 = F(alpha);
                    if(H1<H0)
                    {
                        goto LOOP;
                    }
                }

                // check result
                std::cerr << "Done" << std::endl;
                std::cerr << "C=" << C << std::endl;


            }




            double call_F(double x)
            {
                tao::setprobe(Vtemp,V,x,dV);
                gen_C(Ctemp,Vtemp);
                eqs.updateGamma(Ctemp);
                return COMPUTE_H();
            }

            inline void gen_C(array<double>       &Ctry,
                              const array<double> &Vtry) const throw()
            {
                tao::set(Ctry,Cstar);
                tao::mul_add_trn(Ctry,Q,Vtry);
                SVD::truncate(Ctry);
            }

#if 0
            inline void moveV()
            {
                bool bad = false;
                for(size_t j=M;j>0;--j)
                {
                    beta[j] = 0;
                    if(eqs.active[j])
                    {
                        const double Cj = C[j];
                        if(Cj<0)
                        {
                            beta[j] = -Cj;
                            bad     = true;
                        }
                    }
                }
                if(!bad) return;
                //std::cerr << "beta0=" << beta << std::endl;
                tao::mul(eta,Q,beta);
                //std::cerr << "eta  =" << eta  << std::endl;
                tao::mul_trn(beta,Q,eta);
                //std::cerr << "beta1=" << beta << std::endl;

                // analyze
                double alpha = 0;
                for(size_t j=M;j>0;--j)
                {
                    if(!eqs.active[j]) continue;
                    const double conc_j = C[j];
                    const double beta_j = beta[j];

                    if(conc_j>=0)
                    {
                        if(beta_j<0)
                        {
                            const double atemp = conc_j/(-beta_j);
                            if(alpha<=0)
                            {
                                alpha = atemp;
                            }
                            else
                            {
                                alpha = min_of(alpha,atemp);
                            }
                            std::cerr << "#" << j << ": conc=" << conc_j << ", beta=" << beta_j << ": atemp=" << atemp << " => alpha=" << alpha << std::endl;
                        }
                    }
                    else
                    {
                        assert(conc_j<0);
                        if(beta_j<0)
                        {
                            alpha=0;
                            std::cerr << "#" << j << ": conc=" << conc_j << ", beta=" << beta_j << ": take the risk alpha=0" << std::endl;
                            break;
                        }
                        else
                        {
                            if(beta_j>0)
                            {
                                const double atemp = (-conc_j)/beta_j;
                                if(alpha<=0)
                                {
                                    alpha = atemp;
                                }
                                else
                                {
                                    alpha = min_of(alpha,atemp);
                                }
                                std::cerr << "#" << j << ": conc=" << conc_j << ", beta=" << beta_j << ": atemp=" << atemp << " => alpha=" << alpha << std::endl;
                            }
                        }
                    }
                }

                tao::muladd(V, alpha, eta);
                std::cerr << "conc0=" << C    << std::endl;
                gen_C(C,V);
                tao::mul(V,Q,C);
                std::cerr << "conc1=" << C << std::endl;
                std::cerr << "moveV=" << V << std::endl;
            }
#endif



        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(BootSol);
        };
        
        
        
        void boot::solution(array<double> &C0,
                            equilibria    &eqs,
                            const double t)
        {
            assert(C0.size()>=eqs.M);
            BootSol sol(*this,eqs,*(eqs.pLib),constraints,t);
            for(size_t j=eqs.M;j>0;--j)
            {
                C0[j] = sol.C[j];
            }
            
        }
        
    }
}


