#include "yocto/alchemy/equilibria.hpp"
#include "yocto/code/utils.hpp"
#include "yocto/math/core/tao.hpp"

namespace yocto
{
    using namespace math;

    namespace alchemy
    {

        void equilibria:: balance()
        {
            static const char fn[] = "equilibria.balance: ";
            //std::cerr << "C=" << C << std::endl;
            std::cerr << std::endl << "Balancing..." << std::endl;
            size_t count = 0;
            while(true)
            {
                pLib->display(std::cerr,C);
                //______________________________________________________________
                //
                // counting bad species
                //______________________________________________________________
                size_t nbad = 0;
                for(size_t i=M;i>0;--i)
                {
                    beta[i] = 0;
                    if(active[i]&&C[i]<0)
                    {
                        beta[i] = 1;
                        ++nbad;
                    }
                }
                if(nbad<=0) return;
                std::cerr << "nbad=" << nbad  << std::endl;
                std::cerr << "beta=" << beta << std::endl;

                //______________________________________________________________
                //
                // algebraic eta: descent direction
                //______________________________________________________________
                tao::mul(eta,Nu2,beta);
                std::cerr << "eta=" << eta << std::endl;
                double alpha = 0;
                size_t jzero = 0;

                //______________________________________________________________
                //
                // find greatest possible alpha and species index
                //______________________________________________________________
                for(size_t j=M;j>0;--j)
                {
                    if(active[j])
                    {
                        //______________________________________________________
                        //
                        // what do we do ?
                        //______________________________________________________
                        const double Cj = C[j];
                        const int    ej = eta[j];

                        if(Cj<0)
                        {
                            //__________________________________________________
                            //
                            // bad concentration, go to zero if possible
                            //__________________________________________________
                            if(ej<0)
                            {
                                const string &name = (*pLib)(j)->name;
                                throw exception("%simpossible to balance '%s'",fn,name.c_str());
                            }
                            else
                            {
                                if(ej>0)
                                {
                                    const double atmp = -Cj/ej;
                                    if(alpha>0)
                                    {
                                        if(atmp<alpha)
                                        {
                                            alpha = atmp;
                                            jzero = j;
                                            std::cerr << "updating from bad concentration: jzero=" << jzero << "; alpha=" << alpha << std::endl;
                                        }
                                    }
                                    else
                                    {
                                        jzero = j;
                                        alpha = atmp;
                                        std::cerr << "initializing from BAD concentration jzero=" << jzero << "; alpha=" << alpha << std::endl;
                                    }
                                }
                                // else if eta[j] = 0: do nothing
                            }
                        }
                        else
                        {
                            //__________________________________________________
                            //
                            // good concentration, don't go too low !
                            //__________________________________________________
                            assert(Cj>=0);
                            if(ej<0)
                            {
                                const double atmp = -Cj/ej;
                                if(alpha>0)
                                {
                                    if(atmp<alpha)
                                    {
                                        alpha = atmp;
                                        jzero = j;
                                        std::cerr << "updating from GOOD concentration: jzero=" << jzero << "; alpha=" << alpha << std::endl;
                                    }

                                }
                                else
                                {
                                    jzero = j;
                                    alpha = atmp;
                                    std::cerr << "initializing from GOOD concentration jzero=" << jzero << "; alpha=" << alpha << std::endl;
                                }
                            }
                        }
                    }
                }

                //______________________________________________________________
                //
                // analyzing results
                //______________________________________________________________

                if(jzero<=0)       throw exception("%sno negative concentration can vanish!",fn);
                if(beta[jzero]<=0) throw exception("%slimited by valid concentration of '%s'",fn,(*pLib)(jzero)->name.c_str());

                assert(jzero>0);
                assert(beta[jzero]>0);

                for(size_t j=M;j>0;--j)
                {
                    if(active[j])
                    {
                        if(C[j]<0)
                        {
                            C[j] += alpha * eta[j];
                        }
                        else
                        {
                            C[j] = max_of<double>(0,alpha*eta[j]);
                        }
                    }
                }
                C[jzero] = 0;
                
                if(++count>=2) break;
            }
        }

    }

}

