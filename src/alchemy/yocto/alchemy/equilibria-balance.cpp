#include "yocto/alchemy/equilibria.hpp"
#include "yocto/code/utils.hpp"
#include "yocto/math/core/tao.hpp"

namespace yocto
{
    using namespace math;

    namespace alchemy
    {

        void equilibria:: compute(XiLimits &limits, const array<int> &nu) const throw()
        {
            //__________________________________________________________________
            //
            // Loop over species
            //__________________________________________________________________
            for(size_t j=M;j>0;--j)
            {
                const int nu_j = nu[j];
                if(nu_j>0)
                {
                    const double Xj = max_of<double>(C[j]/nu_j,0.0);
                    std::cerr << "\tproduct,   nu=" << nu_j <<", [" << (*pLib)(j)->name << "]=" << C[j] << ", xi=" << Xj << std::endl;
                    //__________________________________________________________
                    //
                    // this is a product, so there is a reverse limitation
                    //__________________________________________________________
                    XiLimit &rev = limits.reverse;
                    if(!rev.exists)
                    {
                        rev.exists = true;
                        rev.index  = j;
                        rev.value  = Xj;
                    }
                    else
                    {
                        if(Xj<rev.value)
                        {
                            rev.index = j;
                            rev.value = Xj;
                        }
                    }
                    continue;
                }

                if(nu_j<0)
                {
                    const double Xj = max_of<double>(C[j]/(-nu_j),0.0);
                    std::cerr << "\treactant, -nu=" << -nu_j <<", [" << (*pLib)(j)->name << "]=" << C[j] << ", xi=" << Xj << std::endl;
                    //__________________________________________________________
                    //
                    // this is a reactant, so there is a forward limitation
                    //__________________________________________________________
                    XiLimit &fwd = limits.forward;
                    if(!fwd.exists)
                    {
                        fwd.exists = true;
                        fwd.index  = j;
                        fwd.value  = Xj;
                    }
                    else
                    {
                        if(Xj<fwd.value)
                        {
                            fwd.index = j;
                            fwd.value = Xj;
                        }
                    }
                    continue;
                }
            }

            if(limits.forward.exists)
            {
                std::cerr << "\t\tforward limit: " << limits.forward.value << ", reached for " << (*pLib)(limits.forward.index)->name << std::endl;
            }
            else
            {
                std::cerr << "\t\tno forward limit" << std::endl;
            }

            if(limits.reverse.exists)
            {
                std::cerr << "\t\treverse limit: " << limits.reverse.value << ", reached for " << (*pLib)(limits.reverse.index)->name << std::endl;
            }
            else
            {
                std::cerr << "\t\tno reverse limit" << std::endl;
            }


        }


        void equilibria:: balance()
        {
            //static const char fn[] = "equilibria.balance: ";
            //std::cerr << "C=" << C << std::endl;
            std::cerr << std::endl << "Balancing..." << std::endl;

        BALANCE_CYCLE:
            pLib->display(std::cerr,C);
            for(size_t j=M;j>0;--j)
            {
                if(active[j]&&C[j]<0)
                {
                    std::cerr << "bad [" << (*pLib)(j)->name << "]=" << C[j] << std::endl;
                    //__________________________________________________________
                    //
                    // a bad concentration is detected: let's find a reaction
                    // to process it
                    //__________________________________________________________
                    bool processed = false;
                    for(size_t i=N;i>0;--i)
                    {
                        const array<int> &nu   = iNu[i];
                        const int         nu_j = nu[j];
                        if(!nu_j)
                        {
                            std::cerr << "reaction '" << (*this)(i)->name << "' cannot handle " << (*pLib)(j)->name << std::endl;
                            continue; // the reaction cannot handle the species
                        }
                        std::cerr << "querying '" << (*this)(i)->name << "' to process " << (*pLib)(j)->name << std::endl;
                        XiLimits limits;
                        compute(limits,nu);
                    }
                    if(!processed)
                    {
                        const string &id = (*pLib)(j)->name;
                        throw imported::exception("alchemy.equilibria.balance","cannot balance %s",&id[0]);
                    }
                }
            }



        }




    }

}

