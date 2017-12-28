#ifndef YOCTO_GHOSTS_INCLUDED
#define YOCTO_GHOSTS_INCLUDED 1

#include "yocto/ipso/patch.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/core/list.hpp"

namespace yocto
{
    namespace ipso
    {

        //! list of offsets
        typedef vector<coord1D> ghost;



        class ghosts : public object
        {
        public:
            enum direction
            {
                x_lower,
                x_upper,
                y_lower,
                y_upper,
                z_lower,
                z_upper
            };
            typedef core::list_of_cpp<ghosts> list;
            const ghost   send;    //!< built by load
            const ghost   recv;    //!< built by load
            const size_t  count;   //!< built by load
            const size_t  source;  //!< MPI style source
            const size_t  target;  //!< MPI style target
            ghosts       *next;
            ghosts       *prev;
            const direction dir;
            const coord1D   ng;

            explicit ghosts(const direction the_dir,
                            const coord1D the_ng,
                            const size_t  the_source,
                            const size_t  the_target) throw();
            virtual ~ghosts() throw();

            void load(const patch1D &inner,
                      const patch1D &outer,
                      const bool     build);

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(ghosts);
        };
    }
}

#endif

