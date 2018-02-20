#ifndef YOCTO_IPSO_SWAPS_INCLUDED
#define YOCTO_IPSO_SWAPS_INCLUDED 1

#include "yocto/ipso/patch.hpp"
#include "yocto/sequence/vector.hpp"


namespace yocto
{
    namespace ipso
    {
        //! a swap is a list of offsets
        typedef vector<coord1D> swap;

        //! MPI style swaps, local or async
        class swaps : public object
        {
        public:
            static const unsigned lower_x = 0x01;
            static const unsigned upper_x = 0x02;
            static const unsigned lower_y = 0x04;
            static const unsigned upper_y = 0x08;
            static const unsigned lower_z = 0x10;
            static const unsigned upper_z = 0x20;

            static const unsigned flag1D  = lower_x|upper_x;
            static const unsigned flag2D  = lower_y|upper_y;
            static const unsigned flag3D  = lower_z|upper_z;

            typedef core::list_of_cpp<swaps> list;
            
            const swap     recv;      //!< outer indices
            const swap     send;      //!< inner indices
            const size_t   count;     //!< send.size() = recv.size() = count
            const size_t   source;    //!< rank of source
            const size_t   target;    //!< rank of target
            const coord1D  layers;    //!< number of extra layers
            const unsigned pos;       //!< where it is
            swaps        *next;
            swaps        *prev;

            explicit swaps(const size_t   source_rank,
                           const size_t   target_rank,
                           const size_t   some_layers,
                           const unsigned where) throw();
            virtual ~swaps() throw();

            static  unsigned    dim2pos( const size_t dim, const int side ) throw();
            static  const char *pos2txt( const unsigned flag ) throw();

            //! compute the coordinates in 1D, according to settings
            void load(const patch1D &inner,
                      const patch1D &outer,
                      const bool     build);
            

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(swaps);
            void allocate();
        };


    }
}
#endif
