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
            while(true)
            {
                pLib->display(std::cerr,C);
                //______________________________________________________________
                //
                // counting bad species and computing objective gradient
                //______________________________________________________________
                size_t nbad = 0;
                for(size_t i=M;i>0;--i)
                {
                    beta[i] = 0;
                    if(active[i]&&C[i]<0)
                    {
                        beta[i] = -C[i];
                        ++nbad;
                    }
                }
                std::cerr << "nbad=" << nbad  << std::endl;
                if(nbad<=0) return;

                std::cerr << "beta=" << beta  << std::endl;

                //______________________________________________________________
                //
                // compute the descent direction
                //______________________________________________________________
                tao::mul(eta,Nu2,beta);
                std::cerr << "eta="  << eta   << std::endl;

                

                return;
                
            }


        }


        

    }

}

