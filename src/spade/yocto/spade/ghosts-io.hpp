#ifndef YOCTO_SPADE_GHOSTS_IO_INCLUDED
#define YOCTO_SPADE_GHOSTS_IO_INCLUDED 1

#include "yocto/spade/field.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/counted-object.hpp"
#include "yocto/ptr/arc.hpp"
#include "yocto/sort/quick.hpp"

namespace yocto
{
    namespace spade
    {



        class ghosts_io
        {
        public:
            typedef vector<coord1D> indices_type;

            
            class exchange : public counted_object
            {
            public:
                typedef arc_ptr<exchange> pointer;
                const int    peer;
                indices_type recv;  //!< into outer ghosts
                indices_type send;  //!< from inner ghosts

                exchange(const int whom, const size_t n) :
                peer(whom),
                recv(n,as_capacity),
                send(n,as_capacity)
                {
                }

                virtual ~exchange() throw()
                {
                }

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(exchange);
            };


            virtual ~ghosts_io() throw();
            explicit ghosts_io( const field_layouts1D &F );
            explicit ghosts_io( const field_layouts2D &F );


            vector<const exchange::pointer> IO;

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(ghosts_io);

        };

    }
}

#endif
