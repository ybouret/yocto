#ifndef YOCTO_CHEMICAL_EQUILIBRIA_INCLUDED
#define YOCTO_CHEMICAL_EQUILIBRIA_INCLUDED 1

#include "yocto/chemical/equilibrium.hpp"

namespace yocto
{
    namespace chemical
    {
        
        class equilibria :
        public equilibrium::database,
        public counted
        {
        public:
            const string name;

            explicit equilibria(const string &id, const size_t n=0);
            virtual ~equilibria() throw();

            equilibrium & operator()(const string &id, const double K);


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(equilibria);
        };

    }
}


#endif

