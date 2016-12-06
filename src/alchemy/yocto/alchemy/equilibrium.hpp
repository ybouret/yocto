#ifndef YOCTO_ALCHEMY_EQUILIBRIUM_INCLUDED
#define YOCTO_ALCHEMY_EQUILIBRIUM_INCLUDED 1

#include "yocto/alchemy/library.hpp"

namespace yocto
{
    namespace alchemy
    {

        class equilibrium : public counted_object
        {
        public:
            const string name;

            explicit equilibrium( const string &id );
            virtual ~equilibrium() throw();
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(equilibrium);

        };

    }
}

#endif
