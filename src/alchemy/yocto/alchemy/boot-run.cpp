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
                matrix<double> P;     //!< constraint matrix
                vector<double> Lam;   //!< constraint values
                matrix<double> Q;     //!< orthogonal constraint space
                matrix<double> PP;    //!< P*P'
                double         dPP;   //!< det(PP)
                matrix<double> aPP;   //!< adjoint(PP)
                vector<double> Cstar; //!< P'*inv(PP)*Lamn
                vector<double> Utemp;
                const double   t;
                array<double>  &C;
                vector<bool>    used;
                numeric<double>::scalar_field E;
                numeric<double>::vector_field G;
                vector<double>                Vcurr;
                vector<double>                dV;
                vector<double>                Vtemp;
                matrix<double>               &PhiQ;
                numeric<double>::function     gam;

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
                    if(!svd<double>::orthonormal(Q,P)) throw exception("%sunable to build orthonormal space",fn);

                    std::cerr << "Q0=" << Q << std::endl;
                    // TODO: truncate Q ?
                    svd<double>::truncate(Q);
                    std::cerr << "Q=" << Q << std::endl;

                    //__________________________________________________________
                    //
                    // compute Cstar
                    //__________________________________________________________
                    tao::mul(Utemp,aPP,Lam);
                    tao::mul_trn(Cstar,P,Utemp);
                    tao::divby(dPP,Cstar);
                    std::cerr << "Cstar=" << Cstar << std::endl;
                    tao::set(C,Cstar);
                    tao::mul(Vcurr,Q,C);
                    std::cerr << "V0a   =" << Vcurr  << std::endl;
                    for(size_t j=M;j>0;--j)
                    {
                        if(eqs.active[j])
                        {
                            //C[j] += numeric<double>::sqrt_ftol;
                        }
                    }
                    tao::mul(Vcurr,Q,C);
                    std::cerr << "V0b   =" << Vcurr  << std::endl;

                    //__________________________________________________________
                    //
                    // initialize Vcurr from 0
                    //__________________________________________________________
                    moveV(Vcurr);
                    std::cerr << "V1=" << Vcurr << std::endl;
                    std::cerr << "C1=" << C     << std::endl;

                    //__________________________________________________________
                    //
                    // initialize K, Gamma and Phi
                    //__________________________________________________________
                    eqs.computePhi(C,t);
                    std::cerr << "Phi=" << eqs.Phi << std::endl;
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
                    std::cerr << "PhiQ=" << PhiQ << std::endl;
                    if(!LU<double>::build(PhiQ))
                    {
                        throw exception("%ssingular composition",fn);
                    }
                    tao::neg(dV,eqs.Gamma);
                    LU<double>::solve(PhiQ,dV);

                    //__________________________________________________________
                    //
                    // optimize to be sure to decrease the Gamma part...
                    //__________________________________________________________
                    std::cerr << "gam(0)=" << gam(0) << "/" << gam0 << std::endl;

                    triplet<double> xx = { 0,        1.0,   -1.0 };
                    triplet<double> gg = { gam0, gam(xx.b), -1.0 };
                    std::cerr << "expanding..." << std::endl;
                    bracket<double>::expand(gam,xx,gg);
                    std::cerr << "found xx=" << xx << ", gg=" << gg << std::endl;
                    std::cerr << "optimizing..." << std::endl;
                    optimize1D<double>::run(gam,xx,gg,0.0);

                    const double alpha = max_of<double>(xx.b,0.0);
                    std::cerr << "opt=" << xx.b << "/alpha=" << alpha << std::endl;
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
                    eqs.validate(C);
                    std::cerr << "C=" << C << std::endl;

#if 1
                    eqs.updateGamma(C);
                    std::cerr << "Gamma=" << eqs.Gamma << std::endl;
#endif

                    tao::set(Utemp,Lam);
                    tao::mul_sub(Utemp, P,C);
                    const double rms = tao::RMS(Utemp);
                    std::cerr << "rms="   << rms   << std::endl;
                    if(rms>numeric<double>::ftol)
                    {
                        throw exception("%sunable to solve constraints",fn);
                    }
                }


                // move V to get the most valid composition
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

                // generate C from V
                inline void gen_C(const array<double> &V) throw()
                {
                    tao::set(C,Cstar);
                    tao::mul_add_trn(C,Q,V);
                }

                // compute objective function to dectect unvalid concentrations
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

                // compute objective function gradient
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

                // compute gamma along the most positive way
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


        const array<double> & boot:: run( equilibria &eqs, const double t )
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
            // analyse system
            //__________________________________________________________________
            if(Nc<=0)
            {
                // no constraints

                return eqs.C;
            }

            if(Nc>=M)
            {
                // no reactions

                return eqs.C;
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

            return eqs.C;
        }


    }
    
}
