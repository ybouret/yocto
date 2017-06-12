#ifndef YOCTO_FAME_GHOSTS_IO_INCLUDED
#define YOCTO_FAME_GHOSTS_IO_INCLUDED 1

#include "yocto/fame/ghosts.hpp"
#include "yocto/container/cslot.hpp"

namespace yocto
{
    namespace fame
    {

        class ghosts_io
        {
        public:
            explicit ghosts_io() throw();
            virtual ~ghosts_io() throw();

            void ensure(const size_t n);

            const size_t capacity;
            
        private:
            cslot cmem;

            YOCTO_DISABLE_COPY_AND_ASSIGN(ghosts_io);
        };

    }
}


#endif
