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

            //! load data into send_addr
            template <typename FIELD>
            inline size_t load_to_send( const ghosts_pair &gp, const FIELD &F ) const throw()
            {
                assert(capacity>=gp.size*sizeof(typename FIELD::type));
                uint8_t *p = send_addr();
                uint8_t *s = p;
                gp.load(F,p);
                return static_cast<size_t>(p-s);
            }

            


        private:
            cslot  cmem;

            YOCTO_DISABLE_COPY_AND_ASSIGN(ghosts_io);
        };

    }
}


#endif
