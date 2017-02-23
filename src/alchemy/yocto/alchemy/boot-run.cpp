#include "yocto/alchemy/boot.hpp"
#include "yocto/math/core/lu.hpp"
#include "yocto/math/core/svd.hpp"
#include "yocto/math/core/tao.hpp"

#include "yocto/math/opt/bracket.hpp"
#include "yocto/math/opt/minimize.hpp"


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
            matrix<double> iP2(Nc);
            tao::mmul_rtrn(iP2,P,P);
            std::cerr << "P2=" << iP2 << std::endl;
            if(!LU<double>::build(iP2))
            {
                throw exception("%ssingular set of constraints",fn);
            }

            //__________________________________________________________________
            //
            // compute orthogonal space
            //__________________________________________________________________
            matrix<double> Q(N,M);
            if(!svd<double>::orthonormal(Q,P))
            {
                throw exception("%sunable to build orthonormal space",fn);
            }
            std::cerr << "Q0=" << Q << std::endl;
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
            vector<double> Ustar(Lam);
            LU<double>::solve(iP2,Ustar);
            tao::mul_trn(Cstar,P,Ustar);
            std::cerr << "Cstar=" << Cstar << std::endl;

            //__________________________________________________________________
            //
            // prepare memory
            //__________________________________________________________________

            vector<double> V(N), dV(N);
            vector<double> U(Nc), dU(Nc);
            matrix<double> PhiQ(N),PhiP(N,Nc);
            numeric<double>::function F(this, & boot::call_F );


            //__________________________________________________________________
            //
            // initialize
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

            tao::mul(dU,P,C); // save P*C for later
            tao::set(U,dU);   // to compute by LU inversion
            LU<double>::solve(iP2,U);
            tao::mul(V,Q,C);
            std::cerr << "F=" << F0 << std::endl;
            std::cerr << "C=" << C << std::endl;
            std::cerr << "U=" << U << std::endl;
            std::cerr << "V=" << V << std::endl;

            // compute dU
            tao::subp(dU,Lam); // Lam-P*C
            LU<double>::solve(iP2,dU); // dU = inv(P*P')*(Lam-P*C)
            //std::cerr << "dU=" << dU << std::endl;

            // compute dV
            tao::mmul_rtrn(PhiQ,eqs.Phi,Q);
            //std::cerr << "Phi=" << eqs.Phi << std::endl;
            //std::cerr << "PhiQ=" << PhiQ << std::endl;
            if(!LU<double>::build(PhiQ))
            {
                throw exception("%sinvalid Jacobian",fn);
            }
            tao::mmul_rtrn(PhiP,eqs.Phi,P);
            //std::cerr << "PhiP=" << PhiP << std::endl;
            tao::set(dV,eqs.Gamma);
            tao::mul_add(dV,PhiP,dU);
            tao::neg(dV,dV);
            LU<double>::solve(PhiQ,dV);
            //std::cerr << "dV=" << dV << std::endl;

            // save start_C and compute delta_C
            tao::set(start_C,C);
            tao::mul_trn(delta_C,P,dU);
            tao::mul_add_trn(delta_C,Q,dV);
            //std::cerr << "delta_C=" << delta_C << std::endl;

            // find a global minimum of the non linear part
            triplet<double> xx = { 0.0, 1.0, -1 };
            triplet<double> ff = { F0,  F(1.0), -1 };
           // std::cerr << "xx0=" << xx << std::endl; std::cerr << "ff0=" << ff << std::endl;
            bracket<double>::expand(F,xx,ff);
            //std::cerr << "xx1=" << xx << std::endl; std::cerr << "ff1=" << ff << std::endl;
            optimize1D<double>::run(F,xx,ff,0.0);
            //std::cerr << "xx2=" << xx << std::endl; std::cerr << "ff2=" << ff << std::endl;

            const double alpha = max_of<double>(xx.b,0);
            const double F1    = F(alpha);
            std::cerr << "F1=" << F1 << " <-- " << F0 << std::endl;
            std::cerr << "C="  << C  << std::endl;

            if(F1<F0)
            {
                // decreasing
                eqs.updatePhi(C);
                F0 =  F1;
                std::cerr << std::endl;
                goto LOOP;
            }
            else
            {
                // not decreasing, save last C and break
                tao::set(C,start_C);
            }




#if 0
            //__________________________________________________________________
            //
            // initialize C to Cstar, validated
            //__________________________________________________________________
            tao::set(C,Cstar);
            eqs.validate(C);

            //__________________________________________________________________
            //
            // deduce initial V
            //__________________________________________________________________
            tao::mul(V,Q,C);

            //__________________________________________________________________
            //
            // compute initial Gamma, Phi and F
            //__________________________________________________________________

            eqs.computePhi(C,t);
            double F0 = eqs.Gamma2Value();

            std::cerr << "C0=" << C   << std::endl;
            std::cerr << "F0=" << F0  << std::endl;
            std::cerr << "V0=" << V   << std::endl;

        LOOP:
            //__________________________________________________________________
            //
            // compute dV Newton's step, assuming Gamma and Phi are computed
            //__________________________________________________________________
            tao::mmul_rtrn(PhiQ,eqs.Phi,Q);
            if( !LU<double>::build(PhiQ) )
            {
                throw exception("%sunexpected invalid set of concentrations",fn);
            }
            tao::neg(dV,eqs.Gamma);
            LU<double>::solve(PhiQ,dV);

            //__________________________________________________________________
            //
            // initialize optimization: start_C + alpha * delta_C
            //__________________________________________________________________
            tao::set(start_C,C);
            tao::mul_trn(delta_C,Q,dV);

            //__________________________________________________________________
            //
            // get a globally decreasing step
            //__________________________________________________________________
            triplet<double> xx = { 0.0, 1.0, -1 };
            triplet<double> ff = { F0,  F(1.0), -1 };
            bracket<double>::expand(F,xx,ff);
            optimize1D<double>::run(F,xx,ff,0.0);

            const double F1 = F(xx.b);
            std::cerr << "F1=" << F1 << " <-" << F0 << std::endl;
            std::cerr << "C="  << C  << std::endl;

            if(F1<F0)
            {
                // decreasing
                eqs.updatePhi(C);
                F0 =  F1;
                goto LOOP;
            }
            else
            {
                // not decreasing, save last C and break
                tao::set(C,start_C);
            }
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
