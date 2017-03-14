#include "yocto/alchemy/initializer.hpp"

namespace yocto
{
    namespace alchemy
    {

        class InitContext
        {
        public:
            equilibria    &eqs;
            const library &lib;
            const size_t   M;
            const size_t   N;
            const size_t   Nc;

            inline InitContext(equilibria &__eqs) :
            eqs( __eqs ),
            lib( * eqs.pLib ),
            M( lib.size() ),
            N( eqs.size() ),
            Nc(0)
            {

            }

            ~InitContext() throw()
            {
            }
            


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(InitContext);
        };


        void initializer::boot(equilibria     &eqs,
                               vector<double> &C0)
        {

        }

    }

}

