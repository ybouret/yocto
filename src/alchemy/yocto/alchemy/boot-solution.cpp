#include "yocto/alchemy/boot.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/core/svd.hpp"
#include "yocto/math/core/lu.hpp"

#include "yocto/ios/ocstream.hpp"
#include "yocto/math/opt/cgrad.hpp"



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
                // create linear part
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

                // compute Cstar = P'*inv(P*P')*Lam
                {
                    matrix<double> P2(Nc);
                    tao::mmul_rtrn(P2,P,P);
                    std::cerr << "P2=" << P2 << std::endl;
                    if( !LU<double>::build(P2) )
                    {
                        throw exception("%ssingular set of constraints",fn);
                    }
                    vector<double> U(Nc);
                    tao::neg(U,Lam);
                    LU<double>::solve(P2,U);
                    tao::mul_trn(Cstar,P,Lam);
                }
                std::cerr << "Cstar=" << Cstar << std::endl;

                // compute orthonornal matrix
                if( !SVD::orthonormal(Q,P) )
                {
                    throw exception("%sunable to build orthonormal matrix!",fn);
                }
                SVD::truncate(Q);
                std::cerr << "Q=" << Q << std::endl;


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
                        std::cerr << "K_{" << eq.name << "}=" << Ki << ", delta_nu=" << delta << std::endl;
                        if(delta)
                        {
                            const double Ci = pow(Ki,1.0/delta);
                            std::cerr << "\tC_K=" << Ci << std::endl;
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

                std::cerr << "Cmin=" << Cmin << std::endl;
                std::cerr << "Cmax=" << Cmax << std::endl;

                if(Cmax<=0)
                {
                    // 0 is solution...
                    return;
                }

                const double Camp = Cmax - Cmin;

#define COMPUTE_H() (eqs.Gamma2Value())

                // generate initial configuration
                for(size_t j=M;j>0;--j)
                {
                    C[j] = Cmin + ran() * Camp;
                }
                std::cerr << "Cran=" << C << std::endl;
                tao::mul(V,Q,C);
                gen_C(C,V);
                std::cerr << "Cini=" << C << std::endl;
                std::cerr << "Vini=" << V << std::endl;

                if(gen_beta(C))
                {
                    std::cerr << "beta=" << beta << std::endl;
                    std::cerr << "eta="  << eta  << std::endl;
                }



                eqs.updatePhi(C);
                double H0 = COMPUTE_H();
                std::cerr << "H0=" << H0 << std::endl;

                tao::mmul_rtrn(PhiQ, Phi, Q);
                std::cerr << "PhiQ=" << PhiQ << std::endl;
                if( ! LU<double>::build(PhiQ) )
                {
                    std::cerr << "Need to restart..." << std::endl;
                    exit(1);
                }
                tao::neg(dV,Gamma);
                std::cerr << "dV=" << dV << std::endl;
                




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
            }

            inline bool gen_beta(const array<double> &Ctry) throw()
            {
                bool bad = false;
                for(size_t j=M;j>0;--j)
                {
                    beta[j] = 0;
                    if(eqs.active[j])
                    {
                        const double Cj = 0;
                        if(Cj<0)
                        {
                            beta[j] = -Cj;
                            bad = true;
                        }
                    }
                }
                if(bad)
                {
                    tao::mul(eta,Q,beta);
                    return true;
                }
                else
                {
                    return false;
                }
            }


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(BootSol);
        };
        
        
        
        void boot::solution(array<double> &C0,
                            equilibria    &eqs,
                            const double t)
        {
            
            BootSol sol(*this,eqs,*(eqs.pLib),constraints,t);
            
            
        }
        
    }
}


