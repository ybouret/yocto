#ifndef YOCTO_FAME_GHOST_IO_INCLUDED
#define YOCTO_FAME_GHOST_IO_INCLUDED 1

#include "yocto/fame/field.hpp"

namespace yocto
{
    namespace fame
    {

        class ghostIO
        {
        public:
            explicit ghostIO();
            virtual ~ghostIO() throw();


        private:
            size_t capacity;
            YOCTO_DISABLE_COPY_AND_ASSIGN(ghostIO);
        };
    }
}

#endif
