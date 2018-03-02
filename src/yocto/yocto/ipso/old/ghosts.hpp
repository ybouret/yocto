#ifndef YOCTO_GHOSTS_INCLUDED
#define YOCTO_GHOSTS_INCLUDED 1

#include "yocto/ipso/patch.hpp"
#include "yocto/sequence/vector.hpp"

namespace yocto
{
    namespace ipso
    {

        //! a ghost is basically a list of offsets
        typedef vector<coord1D> ghost;


        //! ghots zone data
        class ghosts : public object
        {
        public:
            static const unsigned at_lower_x = 0x01;
            static const unsigned at_upper_x = 0x02;
            static const unsigned at_lower_y = 0x04;
            static const unsigned at_upper_y = 0x08;
            static const unsigned at_lower_z = 0x10;
            static const unsigned at_upper_z = 0x20;


#if 0
            //! the position
            enum position
            {
                x_lower,
                x_upper,
                y_lower,
                y_upper,
                z_lower,
                z_upper
            };
#endif
            typedef core::list_of_cpp<ghosts> list;

            const ghost     send;         //!< built by load if the 'build' flag is set
            const ghost     recv;         //!< built by load if the 'build' flag is set
            const size_t    count;        //!< computed by load, ALWAYS
            const size_t    source;       //!< MPI style source
            const size_t    target;       //!< MPI style target
            const size_t    source_index; //!< source+1, C++ style
            const size_t    target_index; //!< target+1, C++ style
            ghosts         *next;
            ghosts         *prev;
            const unsigned  location;
            const coord1D   ng;
            
            //! build ghosts
            /**
             \param where    ghosts' position
             \param extent     number of linear ghosts to add
             \param the_source MPI style rank of source
             \param the_target MPI style rank of target
             */
            explicit ghosts(const unsigned where,
                            const coord1D  extent,
                            const size_t   the_source,
                            const size_t   the_target) throw();
            virtual ~ghosts() throw();

            //! compute the coordinates in 1D, according to settings
            void load(const patch1D &inner,
                      const patch1D &outer,
                      const bool     build);

            //! compute the coordinates in 2D, according to settings
            void load(const patch2D &inner,
                      const patch2D &outer,
                      const bool     build);

            //! compute the coordinates in 3D, according to settings
            void load(const patch3D &inner,
                      const patch3D &outer,
                      const bool     build);



            static  unsigned    edge2location(const size_t dim, const int s);
            static  const char *location2text(const unsigned p) throw();

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(ghosts);
        };
    }
}

#endif
