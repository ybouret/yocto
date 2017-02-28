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
            pEqs->validate(pEqs->C);

            // compute gamma
            pEqs->updateGamma(pEqs->C);

            // return objective value
            return pEqs->Gamma2Value();
        }
    }
    
}
