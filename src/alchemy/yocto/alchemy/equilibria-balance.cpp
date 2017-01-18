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
            //static const char fn[] = "equilibria.balance: ";
            //std::cerr << "C=" << C << std::endl;
            std::cerr << std::endl << "Balancing..." << std::endl;
            while(true)
            {
                pLib->display(std::cerr,C);

                //______________________________________________________________
                //
                // counting bad species and collect the values
                //______________________________________________________________
                size_t nbad = 0;
                for(size_t j=M;j>0;--j)
                {
                    beta[j] = 0;
                    if(active[j]&&C[j]<0)
                    {
                        beta[j] = -C[j];
                        ++nbad;
                    }
                }
                std::cerr << "nbad=" << nbad  << std::endl;
                if(nbad<=0) return;

                std::cerr << "beta=" << beta  << std::endl;

                //______________________________________________________________
                //
                // Loop over reactions
                //______________________________________________________________
                for(size_t i=N;i>0;--i)
                {
                    const array<int> &nu = iNu[i];
                    std::cerr << "nu#" << i << "=" << nu << std::endl;
                    // compute limits
                    xi_limits limits;
                    size_t    jzr = 0;
                    size_t    jzf = 0;
                    for(size_t j=M;j>0;--j)
                    {
                        const int nu_j = nu[j];
                        if(nu_j>0)
                        {
                            const double      Cj  = max_of<double>(0,C[j]); std::cerr << "C[" << j << "]=" << Cj << ", nu=" << nu_j << std::endl;
                            const double      Xj  = Cj/nu_j;
                            if(!limits.reverse.exists)
                            {
                                limits.reverse.value  = Xj;
                                limits.reverse.exists = true;
                                jzr  = j;
                            }
                            else
                            {
                                if(Xj<limits.reverse.value)
                                {
                                    limits.reverse.value = Xj;
                                    jzr                  = j;
                                }
                            }
                            continue;
                        }

                        if(nu_j<0)
                        {
                            const double      Cj  = max_of<double>(0,C[j]); std::cerr << "C[" << j << "]=" << Cj << ", nu=" << nu_j << std::endl;
                            const double      Xj  = Cj/(-nu_j);
                            if(!limits.reverse.exists)
                            {
                                limits.forward.value  = Xj;
                                limits.forward.exists = true;
                                jzf                   = j;
                            }
                            else
                            {
                                if(Xj<limits.reverse.value)
                                {
                                    limits.reverse.value = Xj;
                                    jzf                  = j;
                                }
                            }

                            continue;
                        }

                    }
                    std::cerr << "..limits#" <<i << ".reverse=" << limits.reverse << ",jzr=" << jzr << std::endl;
                    std::cerr << "..limits#" <<i << ".forward=" << limits.forward << ",jzf=" << jzf << std::endl;


                }

                return;
                
            }


        }


        

    }

}

