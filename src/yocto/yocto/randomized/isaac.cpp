#include "yocto/randomized/isaac.hpp"
#include "yocto/threading/singleton.hpp"

namespace yocto
{

    namespace Randomized
    {

        class BitsSource : public singleton<BitsSource>
        {
        public:

            explicit BitsSource() throw() :
            __simple( ISAAC_INIT_RAND ),
            __crypto( ISAAC_INIT_RAND )
            {
            }

            virtual ~BitsSource() throw()
            {
            }

            ISAAC<4> __simple;
            ISAAC<8> __crypto;

            static const char                 name[];
            static const threading::longevity life_time = 0;

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(BitsSource);
        };

        const char BitsSource::name[] = "Bits";

        Bits & Bits:: Simple()
        {
            return BitsSource::instance().__simple;
        }

        Bits & Bits:: Crypto()
        {
            return BitsSource::instance().__crypto;
        }

    }

}

