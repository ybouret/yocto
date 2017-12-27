#ifndef YOCTO_GHOSTS_INCLUDED
#define YOCTO_GHOSTS_INCLUDED 1

#include "yocto/ipso/patch.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/core/list.hpp"

namespace yocto
{
    namespace ipso
    {

        typedef vector<coord1D> ghost;



        class ghosts : public object
        {
        public:
            typedef core::list_of_cpp<ghosts> list;
            ghost         send;
            ghost         recv;
            ghosts       *next;
            ghosts       *prev;
            const coord1D dim;
            const coord1D ng;

            explicit ghosts(const coord1D d,
                            const coord1D n) throw();
            virtual ~ghosts() throw();

            void load(const patch1D &inner,
                      const patch1D &outer );

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(ghosts);
        };
    }
}

#endif

