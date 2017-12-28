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
            enum position
            {
                x_lower,
                x_upper,
                y_lower,
                y_upper,
                z_lower,
                z_upper
            };
            typedef core::list_of_cpp<ghosts> list;

            const ghost     send;    //!< built by load if the 'build' flag is set
            const ghost     recv;    //!< built by load if the 'build' flag is set
            const size_t    count;   //!< built by load, always
            const size_t    source;  //!< MPI style source
            const size_t    target;  //!< MPI style target
            ghosts         *next;
            ghosts         *prev;
            const position  pos;
            const coord1D   ng;
            
            explicit ghosts(const position the_pos,
                            const coord1D  the_ng,
                            const size_t   the_source,
                            const size_t   the_target) throw();
            virtual ~ghosts() throw();

            void load(const patch1D &inner,
                      const patch1D &outer,
                      const bool     build);

            void load(const patch2D &inner,
                      const patch2D &outer,
                      const bool     build);

            void load(const patch3D &inner,
                      const patch3D &outer,
                      const bool     build);


            static  position dim2pos(const size_t dim, const int s);


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(ghosts);
        };
    }
}

#endif

