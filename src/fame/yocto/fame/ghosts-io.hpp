#ifndef YOCTO_FAME_GHOSTS_IO_INCLUDED
#define YOCTO_FAME_GHOSTS_IO_INCLUDED 1

#include "yocto/fame/fields.hpp"
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

            //! allocate enough memory for n bytes in, n bytes out.
            void ensure(const size_t n);
            
            
            
            uint8_t * recv_addr() const throw();
            uint8_t * send_addr() const throw();

            const size_t capacity; //!< in bytes for in AND out

            //! load data into send_addr, for ONE field
            template <typename FIELD>
            inline size_t load_into_send( const ghosts_pair &gp, const FIELD &F ) const throw()
            {
                assert(capacity>=gp.size*sizeof(typename FIELD::type));
                uint8_t *p = send_addr();
                uint8_t *s = p;
                gp.load(F,p);
                return static_cast<size_t>(p-s);
            }

            //! save data from recv_addr, for ONE FIELD
            template <typename FIELD>
            inline void save_from_recv( const ghosts_pair &gp, FIELD &F ) const throw()
            {
                assert(capacity>=gp.size*sizeof(typename FIELD::type));
                const uint8_t *p = recv_addr();
                gp.save(F,p);
            }
            
            void store_into_send( const ghosts_pair &gp, const fields &fd) const throw();
            
            

        private:
            cslot  cmem;

            YOCTO_DISABLE_COPY_AND_ASSIGN(ghosts_io);
        };

    }
}


#endif
