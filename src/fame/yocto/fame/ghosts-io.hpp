#ifndef YOCTO_FAME_GHOSTS_IO_INCLUDED
#define YOCTO_FAME_GHOSTS_IO_INCLUDED 1

#include "yocto/fame/field3d.hpp"
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

            uint8_t * recv_addr() const throw();
            uint8_t * send_addr() const throw();

            const size_t capacity; //!< in bytes
           

        private:
            size_t half;
            cslot  cmem;

            YOCTO_DISABLE_COPY_AND_ASSIGN(ghosts_io);
        };

    }
}


#endif
