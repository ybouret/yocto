#ifndef YOCTO_ALCHEMY_EQUILIBRIA_INCLUDED
#define YOCTO_ALCHEMY_EQUILIBRIA_INCLUDED 1

#include "yocto/alchemy/equilibrium.hpp"
#include "yocto/container/matrix.hpp"

namespace yocto
{
    namespace alchemy
    {


        class equilibria : public equilibrium::database
        {
        public:
            explicit equilibria();
            virtual ~equilibria() throw();

            matrix<double> Nu;   //!<
            matrix<double> NuT;

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(equilibria);
            size_t max_name_length;
        };
    }
}

#endif
