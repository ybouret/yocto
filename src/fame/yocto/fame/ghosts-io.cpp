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
        half(0),
        cmem()
        {
        }

        void ghosts_io:: ensure(const size_t n)
        {
            if(n>capacity)
            {
                const size_t tmp_half = memory::align(n);
                const size_t tmp_capa = tmp_half * 2;
                cmem.prepare(tmp_capa);
                (size_t&)capacity = tmp_capa;
                half              = tmp_half;
            }
            
        }


    }
}

