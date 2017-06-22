#include "yocto/fame/field.hpp"


namespace yocto
{
    namespace fame
    {
        void ghosts_pair:: stream_load( const field_data &F, uint8_t * &p ) const throw()
        {
            const ghost_type &g         = inner;
            const size_t      item_size = F.item_size;
            for(size_t i=size;i>0;--i)
            {
                const void *ptr = F.address_of( g[i] );
                memcpy(p,ptr,item_size);
                p += item_size;
            }

        }

        void ghosts_pair:: stream_save( field_data &F, const uint8_t * &p ) const throw()
        {
            const ghost_type &g         = outer;
            const size_t      item_size = F.item_size;
            for(size_t i=size;i>0;--i)
            {
                void *ptr =  F.address_of( g[i] );
                memcpy(ptr,p,item_size);
                p += item_size;
            }

        }
    }

}

