#ifndef YOCTO_FAME_GHOST_IO_INCLUDED
#define YOCTO_FAME_GHOST_IO_INCLUDED 1

#include "yocto/fame/field.hpp"
#include "yocto/code/bmove.hpp"

namespace yocto
{
    namespace fame
    {

        class ghostIO
        {
        public:
            explicit ghostIO();
            virtual ~ghostIO() throw();

            template <typename FIELD>
            inline void read_from( const FIELD &F, const ghost &g, uint8_t * &p )
            {
                for(size_t i=g.size();i>0;--i)
                {
                    const size_t idx = g[i];
                    const void  *ptr = & F.entry[ idx ];
                    bmove<sizeof( typename FIELD::type )>(p,ptr);
                    p +=  sizeof( typename FIELD::type );
                }
            }

        private:
            size_t capacity;
            YOCTO_DISABLE_COPY_AND_ASSIGN(ghostIO);
        };
    }
}

#endif
