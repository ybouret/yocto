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

                    //__________________________________________________________
                    //
                    // compute limits
                    //__________________________________________________________

                    bool   has_reverse = false;
                    double max_reverse = 0;
                    size_t jzr         = 0;

                    bool   has_forward = false;
                    double max_forward = 0;
                    size_t jzf         = 0;

                    for(size_t j=M;j>0;--j)
                    {
                        const int nu_j = nu[j];
                        if(nu_j>0)
                        {
                            const double Xj = max_of<double>(C[j]/nu_j,0.0);
                            std::cerr << "X[" << j << "]=" << Xj << ", nu=" << nu_j << std::endl;
                            //__________________________________________________
                            //
                            // this is a product, so there is a reverse limitation
                            //__________________________________________________
                            if(!has_reverse)
                            {
                                jzr         = j;
                                has_reverse = true;
                                max_reverse = Xj;
                            }
                            else
                            {
                                if(Xj<max_reverse)
                                {
                                    jzr         = j;
                                    max_reverse = Xj;
                                }
                            }
                            continue;
                        }

                        if(nu_j<0)
                        {
                            const double Xj = max_of<double>(C[j]/(-nu_j),0.0);
                            std::cerr << "X[" << j << "]=" << Xj << ", nu=" << nu_j << std::endl;
                            //__________________________________________________
                            //
                            // this is a reactant, so there is a forward limitation
                            //__________________________________________________
                            if(!has_forward)
                            {
                                jzf         = j;
                                has_forward = true;
                                max_forward = Xj;
                            }
                            else
                            {
                                if(Xj<max_forward)
                                {
                                    jzf         = j;
                                    max_forward = Xj;
                                }
                            }
                        }
                    }

                    if(has_reverse)
                    {
                        std::cerr << "max_reverse=" << max_reverse << ", jzr=" << jzr << std::endl;
                    }
                    else
                    {
                        std::cerr << "no reverse" << std::endl;
                    }

                    if(has_forward)
                    {
                        std::cerr << "max_forward=" << max_forward << ", jzf=" << jzr << std::endl;
                    }
                    else
                    {
                        std::cerr << "no forward" << std::endl;
                    }
                    std::cerr << std::endl;



                }


                return;
                
            }


        }


        

    }

}

