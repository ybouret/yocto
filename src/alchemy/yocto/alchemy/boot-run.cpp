#include "yocto/alchemy/boot.hpp"
#include "yocto/math/core/lu.hpp"
#include "yocto/math/core/svd.hpp"
#include "yocto/math/core/tao.hpp"

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

            }


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
            // compute Ustar
            //__________________________________________________________________
            vector<double> Ustar(Lam);
            vector<double> Cstar(M);
            LU<double>::solve(iP2,Ustar);
            tao::mul_trn(Cstar, P, Ustar);
            std::cerr << "Ustar=" << Ustar << std::endl;
            std::cerr << "Cstar=" << Cstar << std::endl;

            vector<double> U(Nc), dU(Nc);
            vector<double> V(N),  dV(N);
            matrix<double> PhiQ(N);
            vector<double> C_old(M);
            bool first = true;
            size_t count = 0;

        LOOP:
            // initialize U, compute C from U and V
            tao::set(C_old,C);
            tao::set(U,Ustar);
            tao::set(C,Cstar);
            tao::mul_add_trn(C,Q,V);
            std::cerr << "C0=" << C << std::endl;
            std::cerr << "U0=" << U << std::endl;
            std::cerr << "V0=" << V << std::endl;

            // validate C
            eqs.validate();

            // recompute U and V
            tao::mul(U,P,C);
            LU<double>::solve(iP2,U);
            tao::mul(V,Q,C);
            std::cerr << "C1=" << C << std::endl;
            std::cerr << "U1=" << U << std::endl;
            std::cerr << "V1=" << V << std::endl;

            // compute dV from Gamma
            if(first)
            {
                first = false;
                eqs.computePhi(C, t);
            }
            else
            {
                eqs.updatePhi(C);
            }

            tao::mmul_rtrn(PhiQ,eqs.Phi,Q);
            std::cerr << "PhiQ=" << PhiQ << std::endl;
            if( !LU<double>::build(PhiQ) )
            {
                throw exception("%ssingular set of constraints during computation",fn);
            }
            tao::neg(dV,eqs.Gamma);
            LU<double>::solve(PhiQ,dV);
            std::cerr << "dV=" << dV << std::endl;
            //update V
            tao::add(V,dV);


            // update C
            tao::mul_trn(C,P,U);
            tao::mul_add_trn(C,Q,V);
            std::cerr << "C2=" << C << std::endl;
            std::cerr << "U2=" << U << std::endl;
            std::cerr << "V2=" << V << std::endl;


            // recompute U and V
            eqs.validate();
            std::cerr << "V3=" << V << std::endl;
            std::cerr << "C3   =" << C     << std::endl;
            std::cerr << "C_old=" << C_old << std::endl;

            //tao::mul(U,P,C);
            //LU<double>::solve(iP2,U);
            tao::mul(V,Q,C);
            //std::cerr << "U3=" << U << std::endl;
            if(++count<=50) goto LOOP;

        }


    }

}
