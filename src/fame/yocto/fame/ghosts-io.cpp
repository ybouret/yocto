#include "yocto/fame/ghosts-io.hpp"

namespace yocto
{
    namespace fame
    {

        ghosts_io:: ~ghosts_io() throw()
        {
            (size_t &)capacity = 0;
        }

        ghosts_io:: ghosts_io() throw() :
        capacity(0),
        cmem()
        {
        }

        void ghosts_io:: ensure(const size_t n)
        {
            if(n>capacity)
            {
                const size_t tmp_capa = memory::align(n);
                const size_t tmp_full = tmp_capa * 2;
                if(cmem.size<tmp_full)
                {
                    cslot ctmp(tmp_full);
                    cmem.swap_with(ctmp);
                }
                assert(cmem.size>=tmp_full);
                (size_t&)capacity = tmp_capa;
            }
            
        }

        uint8_t * ghosts_io:: recv_addr() const throw()
        {
            assert(cmem.size>=capacity*2);
            return (uint8_t *)(cmem.data);
        }

        uint8_t * ghosts_io:: send_addr() const throw()
        {
            assert(cmem.size>=capacity*2);
            return ((uint8_t *)(cmem.data)+capacity);
        }

        void ghosts_io:: store_into_send(const ghosts_pair &gp,
                                         const fields      &fd) const throw()
        {
            assert(capacity>=gp.size*fd.block_size);
            uint8_t *              p = send_addr();
            fields::const_iterator i = fd.begin();
            for(size_t n=fd.size();n>0;--n,++i)
            {
                gp.stream_load(**i,p);
            }
        }

        
    }
}

