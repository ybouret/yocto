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
            {
                vector<double> U(Lam);
                LU<double>::solve(iP2,U);
                tao::mul_trn(Cstar,P,U);
            }
            std::cerr << "Cstar=" << Cstar << std::endl;

            vector<double> V(N), dV(N);
            matrix<double> PhiQ(N);
            bool first = true;


            //__________________________________________________________________
            //
            // initialize
            //__________________________________________________________________

            tao::set(C,Cstar);
            tao::mul_add_trn(C,Q,V);
            std::cerr << "V0=" << V << std::endl;
            std::cerr << "C0=" << C << std::endl;
            eqs.validate();
            tao::mul(V,Q,C);
            std::cerr << "V1=" << V << std::endl;
            std::cerr << "C1=" << C << std::endl;


            size_t count = 0;
            numeric<double>::function F(this, & boot::call_F );
        LOOP:
            //__________________________________________________________________
            //
            // update V by non linear constraints
            //__________________________________________________________________
            if(first)
            {
                first = false;
                eqs.computePhi(C,t);
            }
            else
            {
                eqs.updatePhi(C);
            }

            //__________________________________________________________________
            //
            // compute the Newton step
            //__________________________________________________________________
            tao::mmul_rtrn(PhiQ,eqs.Phi,Q);
            if(!LU<double>::build(PhiQ))
            {
                throw exception("%ssingular concentrations",fn);
            }
            tao::neg(dV,eqs.Gamma);
            LU<double>::solve(PhiQ,dV);
            //std::cerr << "dV=" << dV << std::endl;

            //__________________________________________________________________
            //
            // prepare to decrease
            //__________________________________________________________________
            tao::set(start_C,C);
            tao::mul_trn(delta_C,Q,dV);

            triplet<double> xx = { 0.0,                 1.0,         0 };
            triplet<double> ff = { pEqs->Gamma2Value(), call_F(1.0), 0 };
            std::cerr << "xx0=" << xx << std::endl; std::cerr << "ff0=" << ff << std::endl;
            bracket<double>::expand(F,xx,ff);

            std::cerr << "xx1=" << xx << std::endl;  std::cerr << "ff1=" << ff << std::endl;
            optimize1D<double>::run(F,xx,ff, 0.0);
            std::cerr << "xx2=" << xx << std::endl;  std::cerr << "ff2=" << ff << std::endl;


            //__________________________________________________________________
            //
            // compute the new C
            //__________________________________________________________________
            tao::setprobe(C, start_C, xx.b, delta_C);
            std::cerr << "C_opt=" << C << std::endl;
            eqs.validate();
            tao::mul(V,Q,C);
            std::cerr << "C_fin=" << C << std::endl;
            std::cerr << "V_fin=" << V << std::endl;
            if(++count<=10) goto LOOP;
        }

        double boot:: call_F(double alpha)
        {
            assert(pEqs);
            tao::setprobe(pEqs->C, start_C, alpha, delta_C);
            pEqs->updateGamma(pEqs->C);
            return pEqs->Gamma2Value();
        }
    }

}
