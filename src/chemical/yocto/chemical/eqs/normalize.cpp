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
        static const char fn[] = "equilibria.normalize: ";
        
        double equilibria:: __normGamma(double alpha)
        {
            for(size_t j=M;j>0;--j)
            {
                const double Cj = (Ctry[j]=C[j]+alpha*dC[j]);
                if(active[j]&&(Cj<0))
                {
                    Ctry[j] = 0;
                }
            }
            updateGamma(Ctry);
            return GammaToScalar();
        }

        static inline
        void __optimize(math::numeric<double>::function &F,
                        triplet<double>                 &XX,
                        triplet<double>                 &FF) throw()
        {

            bracket<double>::inside(F, XX, FF);

            double width = XX.c-XX.a; assert(width>=0);
            while(true)
            {
                kernel::minimize(F, XX, FF);
                const double new_width = XX.c-XX.a;
                assert(new_width<=width);
                if(new_width>=width)
                {
                    break;
                }
                width = new_width;
            }

        }

        void equilibria:: compute_full_step()
        {
            tao::mmul_rtrn(W,Phi,Nu);
            std::cerr << "W=" << W << std::endl;
            for(size_t i=1;i<=N;++i)
            {
                std::cerr << "Phi*Nu#" << i << "=" << tao::dot(Phi[i],Nu[i]) << ", Gamma=" << Gamma[i] << std::endl;
            }
            exit(0);
            if(!LU<double>::build(W))
            {
                throw exception("%ssingular set of concentrations",fn);
            }
            tao::neg(xi,Gamma);
            LU<double>::solve(W,xi);
            tao::mul(dC,NuT,xi);
            std::cerr << "xi  =" << xi << std::endl;
            std::cerr << "dC  =" << dC << std::endl;
        }


        void equilibria:: normalize(array<double> &C0, const double t)
        {
            // transfer
            for(size_t j=M;j>0;--j)
            {
                C[j] = C0[j];
                if(active[j]&&C[j]<0)
                {
                    throw exception("%sinvalid initial concentration",fn);
                }
            }

#if 0
            {
                iterator ii = begin();
                for(size_t i=1;i<=N;++i,++ii)
                {
                    const equilibrium &eq = **ii;
                    const double Kt = (K[i] = eq.K(i));
                    try_solve(i,eq,Kt);
                }

            }
            exit(0);
#endif


            // initialize K, Gamma, Phi, and GS
            initializeGammaAndPhi(C,t);
            double GS = GammaToScalar();

            while(true)
            {
                // compute full step
                std::cerr << "Cini =" << C << std::endl;
                std::cerr << "Gamma=" << Gamma << std::endl;
                compute_full_step();

                // limit full step, stay positive
                double alpha = 1;
                size_t zindx = 0;
                for(size_t j=M;j>0;--j)
                {
                    if(!active[j]) { assert(Fabs(dC[j])<=0); continue; }
                    const double d = dC[j];
                    if(d<0)
                    {
                        const double c = C[j]; assert(c>=0);
                        if(c+d<=0)
                        {
                            const double atmp=c/(-d);
                            if(atmp<alpha)
                            {
                                alpha = atmp;
                                zindx = j;
                            }
                        }
                    }
                }
                std::cerr << "alpha=" << alpha << ", zindx=" << zindx << std::endl;
                for(size_t j=M;j>0;--j)
                {
                    if(active[j])
                    {
                        Ctry[j] = max_of<double>(C[j]+alpha*dC[j],0);
                        dC[j] = Ctry[j] - C[j];
                    }
                    else
                    {
                        Ctry[j] = C[j];
                        dC[j]   = 0;    // should be
                    }
                }
                if(zindx)
                {
                    Ctry[zindx] = 0;
                    dC[zindx]   = Ctry[zindx] - C[zindx];
                }

                // check where we are
                updateGamma(Ctry);
                double gs = GammaToScalar();
                std::cerr << "|Gamma| : " << GS << " -> " << gs << std::endl;
                if(gs<=0)
                {
                    GS=0;
                    break;
                }

                if(gs<GS)
                {
                    tao::set(C,Ctry);
                    updatePhi(C);
                    GS=gs;
                    continue;
                }

                std::cerr << "Need to backtrack!" << std::endl;
                triplet<double> xx = { 0, alpha,alpha };
                triplet<double> ff = { GS,gs,   gs    };
                __optimize(normGamma,xx,ff);
                gs = normGamma(alpha=xx.b);
                std::cerr << "gs=" << gs << "@alpha=" << alpha << std::endl;
                exit(0);
                break;

            }

            if(GS>0)
            {
                // check convergence
                std::cerr << "Checking convergence..." << std::endl;
            }
        }


        bool equilibria:: try_solve(const size_t i, const equilibrium &eq, const double Kt)
        {
            equilibrium::range rng;
            std::cerr << "Try Solve " << eq.name << std::endl;
            array<double> &nu  = Nu[i];
            array<double> &phi = Phi[i];

            eq.computeGradient(phi,C,Kt);
            std::cerr << "phi=" << phi << std::endl;
            std::cerr << "nu =" << nu  << std::endl;
            const double   den = tao::dot(phi,nu);
            if(Fabs(den)<=0)
            {
                return false;
            }
            const double gam    = eq.computeGamma(C,Kt);
            const double extent = -gam/den;
            std::cerr << "extent=" << extent << std::endl;
            if(extent>0)
            {
                eq.compute_forward(rng,C);
                std::cerr << "fwd=" << rng << std::endl;
            }
            else
            {
                if(extent<0)
                {
                    eq.compute_reverse(rng,C);
                    std::cerr << "rev=" << rng << std::endl;
                }
                else
                {
                    return true;
                }
            }

            return false;
        }


    }

}

