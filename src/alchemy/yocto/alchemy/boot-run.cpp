#include "yocto/alchemy/boot.hpp"
#include "yocto/math/core/lu.hpp"
#include "yocto/math/core/svd.hpp"
#include "yocto/math/core/tao.hpp"

#include "yocto/math/opt/bracket.hpp"
#include "yocto/math/opt/minimize.hpp"

#include "yocto/math/core/adjoint.hpp"


namespace yocto
{

    using namespace math;

    namespace alchemy
    {

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

            array<double> &C = eqs.C;

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
            // check the rank and inv(P*P')
            //__________________________________________________________________
            matrix<double> P2(Nc,Nc);
            tao::mmul_rtrn(P2,P,P);
            std::cerr << "P2=" << P2 << std::endl;
            const double dPP = ideterminant(P2);
            if(Fabs(dPP)<0) throw exception("%singular constraints",fn);
            std::cerr << "dPP=" << dPP << std::endl;
            matrix<double> aPP(Nc,Nc);
            iadjoint(aPP,P2);
            std::cerr << "aPP=" << aPP << std::endl;


            //__________________________________________________________________
            //
            // compute orthogonal space
            //__________________________________________________________________
            matrix<double> Q(N,M);
            if(!svd<double>::orthonormal(Q,P))
            {
                throw exception("%sunable to build orthonormal space",fn);
            }
            for(size_t i=1;i<=N;++i)
            {
                svd<double>::truncate(Q[i]);
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
            vector<double> V(N),  dV(N);
            vector<double> U(Nc), dU(Nc);
            matrix<double> PhiQ(N),PhiP(N,Nc);
            numeric<double>::function F(this, & boot::call_F );


            //__________________________________________________________________
            //
            // initialize C to a validated Cstar
            //__________________________________________________________________
            tao::set(C,Cstar);
            eqs.validate(C);
            eqs.computePhi(C,t);
            double F0 = eqs.Gamma2Value();

        LOOP:

            //__________________________________________________________________
            //
            // deduce components of a validated C, assuming Gamma,Phi and F0
            // are also computed
            //__________________________________________________________________

            tao::mul(Utmp,P,C);               // save P*C for later
            tao::mul_and_div(U,aPP,Utmp,dPP); // U = inv(P*P')*(P*C)
            tao::mul(V,Q,C);
            std::cerr << "F=" << F0 << std::endl;
            std::cerr << "C=" << C << std::endl;
            std::cerr << "U=" << U << std::endl;
            std::cerr << "V=" << V << std::endl;

            //__________________________________________________________________
            //
            // compute dU = inv(P*P')*(Lam-P*C)
            //__________________________________________________________________
            tao::subp(Utmp,Lam);               // Lam-P*C
            tao::mul_and_div(dU,aPP,Utmp,dPP); // dU = inv(P*P')*(Lam-P*C)

            //__________________________________________________________________
            //
            // compute dV = -inv(Phi*Q')*(Gamma+Phi*P'*dU)
            //__________________________________________________________________
            tao::mmul_rtrn(PhiQ,eqs.Phi,Q);
            if(!LU<double>::build(PhiQ))
            {
                throw exception("%sinvalid Jacobian",fn);
            }
            tao::mmul_rtrn(PhiP,eqs.Phi,P);
            tao::set(dV,eqs.Gamma);
            tao::mul_add(dV,PhiP,dU);
            tao::neg(dV,dV);
            LU<double>::solve(PhiQ,dV);


            //__________________________________________________________________
            //
            // save start_C and compute delta_C
            //__________________________________________________________________
            tao::set(start_C,C);
            tao::mul_trn(delta_C,P,dU);
            tao::mul_add_trn(delta_C,Q,dV);

            //__________________________________________________________________
            //
            // find a global minimum of the non linear part, avoiding
            // oscillations
            //__________________________________________________________________
            triplet<double> xx = { 0.0, 1.0, -1 };
            triplet<double> ff = { F0,  F(1.0), -1 };
            bracket<double>::expand(F,xx,ff);
            optimize1D<double>::run(F,xx,ff,0.0);

            const double alpha = max_of<double>(xx.b,0);
            const double F1    = F(alpha);
            std::cerr << "F1=" << F1 << " <-- " << F0 << std::endl;
            std::cerr << "C="  << C  << std::endl;

            if(F1<F0)
            {
                //______________________________________________________________
                //
                // decreasing
                //______________________________________________________________
                eqs.updatePhi(C);
                F0 =  F1;
                std::cerr << std::endl;
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

            
#if 0
            //__________________________________________________________________
            //
            // clean up
            //__________________________________________________________________
            for(size_t k=0;k<10;++k)
            {
                //tao::set(dU,Lam);
                //tao::mul_sub(dU,P,C);
                //LU<double>::solve(iP2,dU);
                tao::set(Utmp,Lam);
                tao::mul_sub(Utmp,P,C);
                tao::mul_and_div(dU,aPP,Utmp,dPP);

                tao::mul_trn(delta_C, P, dU);
                //std::cerr << "delta_U=" << dU      << std::endl;
                std::cerr << "delta_C=" << delta_C << std::endl;
                tao::add(C,delta_C);
                eqs.validate(C);
            }
            std::cerr << "C0=" << C << std::endl;
            svd<double>::truncate(C);
            std::cerr << "C1=" << C << std::endl;
#endif
            
        }

        double boot:: call_F(double alpha)
        {
            assert(pEqs);
            // create a probe
            tao::setprobe(pEqs->C, start_C, alpha, delta_C);

            // clamp the probe
            pEqs->validate(pEqs->C);

            // compute gamma
            pEqs->updateGamma(pEqs->C);

            // return objective value
            return pEqs->Gamma2Value();
        }
    }
    
}
