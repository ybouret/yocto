#include "yocto/chemical/equilibria.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/math/core/adjoint.hpp"

namespace yocto
{
    using namespace math;
    
    namespace chemical
    {
        

        bool equilibria:: balance( array<double> &C0 ) throw()
        {
            //__________________________________________________________________
            //
            //
            // Initial Check: compute E0 and possible beta
            //
            //__________________________________________________________________
        BALANCE:
            double E0 = 0;
            for(size_t j=M;j>0;--j)
            {
                const double Cj = C0[j];
                if(active[j]&&(Cj<0))
                {
                    E0     +=  Cj*Cj;
                    beta[j] = -Cj;
                }
                else
                {
                    beta[j] = 0;
                }

            }
            //std::cerr << "E0=" << E0/2 << std::endl;
            if(E0<=0)
            {
                // everything is fine
                std::cerr << "\t<balanced>" << std::endl;
                //stay_positive();
                return true;
            }

            //__________________________________________________________________
            //
            //
            // computing increases and check them
            //
            //__________________________________________________________________
            {
                //______________________________________________________________
                //
                // compute the concentration descent direction
                //______________________________________________________________

                tao::mul(dC,nu2,beta);
                std::cerr << "beta=" << beta << std::endl;
                std::cerr << "dC  =" << dC << std::endl;
                //______________________________________________________________
                //
                // forward optimize
                //______________________________________________________________
                double amax = 0; //!< limitation to decrease concentration
                double jmax = 0; //!< the limiting index

                double amin = 0; //!< limitation to increase negative conc
                double jmin = 0; //!< the limiting index

                for(size_t j=M;j>0;--j)
                {
                    const double c = C0[j];
                    const double d = dC[j];
                    //std::cerr << c << " -> " << d << std::endl;
                    if(d<0)
                    {
                        //______________________________________________________
                        //
                        // find limiting species with decreasing concentration
                        //______________________________________________________
                        const double atmp = c/(-d);
                        //std::cerr << "\tamax=" << atmp << "@" << j << std::endl;
                        if(jmax<=0)
                        {
                            amax = atmp;
                            jmax = j;
                        }
                        else
                        {
                            if(atmp<amax)
                            {
                                amax = atmp;
                                jmax = j;
                            }
                        }
                    }
                    else
                    {
                        //______________________________________________________
                        //
                        // find minimal possible species with increasing conc
                        //______________________________________________________
                        assert(d>=0);
                        if(d>0)
                        {
                            if(c<0)
                            {
                                const double atmp = (-c)/d;
                                //std::cerr << "\tamin=" << atmp << "@" << j << std::endl;
                                if(jmin<=0)
                                {
                                    amin = atmp;
                                    jmin = j;
                                }
                                else
                                {
                                    if(atmp>amin)
                                    {
                                        amin = atmp;
                                        jmin = j;
                                    }
                                }
                            }
                        }
                    }
                }
                std::cerr << "amax=" << amax << "@" << jmax << std::endl;
                std::cerr << "amin=" << amin << "@" << jmin << std::endl;
                if( (jmax<=0) && (jmin<=0) )
                {
                    return false;
                }

                if(jmax>0)
                {
                    //__________________________________________________________
                    //
                    // decreasing limitation
                    //__________________________________________________________
                    if(amax<=0)
                    {
                        std::cerr << "balancing is blocked" << std::endl;
                        return false;
                    }
                    else
                    {
                        //______________________________________________________
                        //
                        // update
                        //______________________________________________________
                        std::cerr << "maximum advance" << std::endl;
                        tao::muladd(C0,amax,dC);
                        C0[jmax] = 0;
                        std::cerr << "C0=" << C0 << std::endl;
                        goto BALANCE;
                    }
                }
                else
                {
                    //__________________________________________________________
                    //
                    // no decreasing limitation
                    //__________________________________________________________
                    assert(jmin>0);
                    std::cerr << "minimum advance" << std::endl;
                    tao::muladd(C0,amin,dC);
                    C0[jmin] = 0;
                    std::cerr << "C0=" << C0 << std::endl;
                    goto BALANCE;
                }

            }
        }

        bool equilibria:: balance2(array<double> &C0) throw()
        {
            matrix<double> nn(N,N);
            tao::mmul_rtrn(nn,nu,nu);
            std::cerr << "nn=" << nn << std::endl;
            double det_nn = ideterminant(nn);
            std::cerr << "det_nn=" << det_nn << std::endl;
            matrix<double> adj_nn(N,N);
            iadjoint(adj_nn,nn);
            std::cerr << "adj_nn=" << adj_nn << std::endl;
            matrix<double> tmp(N,M);
            tao::mmul(tmp,adj_nn,nu);
            matrix<double> pNu(M,M);
            tao::mmul(pNu,nuT,tmp);
            std::cerr << "pNu=" << pNu << std::endl;
            std::cerr << "C0=" << C0 << std::endl;
            bool bad = false;
            for(size_t j=M;j>0;--j)
            {
                const double Cj = C0[j];
                if(active[j]&&(Cj<0))
                {
                    bad     = true;
                    beta[j] = -Cj;
                }
            }
            if(bad)
            {
                std::cerr << "beta=" << beta << std::endl;
            }
            else
            {
                std::cerr << "OK" << std::endl;
            }
#if 0
            assert(C0.size()==M);
            matrix<double> sigma;
            size_t         dof = 0;
            if(M>N)
            {
                dof = M-N;
                sigma.make(dof,M);
                if(!svd<double>::orthonormal(sigma,nu))
                {
                    std::cerr << "couldn't build sigma" << std::endl;
                }
                std::cerr << "nu    = " << nu << std::endl;
                std::cerr << "sigma = " << sigma << std::endl;
            }
            
            bool bad = false;
            for(size_t j=M;j>0;--j)
            {
                const double Cj = C0[j];
                if(active[j]&&(Cj<0))
                {
                    bad     = true;
                    beta[j] = -Cj;
                }
            }
            std::cerr << "C0=" << C0 << std::endl;
            if(bad)
            {
                std::cerr << "beta=" << beta << std::endl;
                vector<double> p(N);
                tao::mul(p,nu,beta);
                std::cerr << "p=" << p << std::endl;
                if(dof)
                {
                    vector<double> q(dof);
                    tao::mul(q,sigma,beta);
                    std::cerr << "q=" << q << std::endl;
                }
            }
            else
            {
                std::cerr << "OK" << std::endl;
            }
            return false;
#endif
        }
        
    }

}

