#include "yocto/alchemy/equilibria.hpp"


namespace yocto
{
    using namespace math;

    namespace alchemy
    {


        void equilibria:: init0( array<double> &C0 )
        {
            assert(C0.size()>=M);
            static const char fn[] = "equilibria:init0: ";

            vector<equilibrium::pointer> sub(N,as_capacity);
            for( iterator it = begin(); it != end(); ++it)
            {
                const equilibrium::pointer &pEq = *it;
                if(pEq->number_of_species()<=0)
                {
                    throw exception("%sinvalid equilibrium",fn);
                }
            }


        }


    }

}

