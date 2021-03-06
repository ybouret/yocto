#ifndef YOCTO_IPSO_SWAPS_INCLUDED
#define YOCTO_IPSO_SWAPS_INCLUDED 1

#include "yocto/ipso/patch.hpp"
#include "yocto/ipso/swap-buffers.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/counted-object.hpp"
#include "yocto/sequence/addr-list.hpp"
#include "yocto/sort/merge.hpp"
#include "yocto/core/list.hpp"

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

            static const unsigned xup_yup = upper_x | upper_y;
            static const unsigned xup_ylo = upper_x | lower_y;
            static const unsigned xlo_ylo = lower_x | lower_y;
            static const unsigned xlo_yup = lower_x | upper_y;

            static const unsigned diagonal1 = 0x00;
            static const unsigned diagonal2 = 0x01;

            static const unsigned flag1D  = lower_x|upper_x;
            static const unsigned flag2D  = lower_y|upper_y;
            static const unsigned flag3D  = lower_z|upper_z;

            typedef core::list_of_cpp<swaps> list;
            
            const swap           recv;      //!< outer indices
            const swap           send;      //!< inner indices
            const size_t         count;     //!< send.size() = recv.size() = count
            mutable swap_buffers iobuf;     //!< local memory for send and recv
            const size_t         source;    //!< rank of source
            const size_t         target;    //!< rank of target
            const coord1D        layers;    //!< number of extra layers
            const unsigned       pos;       //!< where it is
            swaps               *next;
            swaps               *prev;

            explicit swaps(const size_t   source_rank,
                           const size_t   target_rank,
                           const size_t   some_layers,
                           const unsigned where) throw();
            virtual ~swaps() throw();

            static  unsigned    dim2pos( const size_t dim, const int side ) throw();
            static  const char *pos2txt( const unsigned flag ) throw();
            static  string      flg2str( const unsigned flags);
            static  unsigned    diagonal_for( const unsigned flags );
            

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

            //! allocate I/O memory for block_size
            void allocate_for( const size_t block_size );

            //! check if was built...
            bool built() const throw();

            //! for sorting
            static int compare_by_pos(const swaps *lhs, const swaps *rhs, void *) throw();

            //! deduce the equivalent 1D swaps
            swaps *clone1D() const;

            //! allocate memory
            void allocate();

            //! co-sort indices
            void io_check() throw();


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(swaps);
        };

#define YOCTO_IPSO_SWAP_LOAD()  \
allocate();                     \
swap    & _recv = (swap &)recv; \
swap    & _send = (swap &)send;

        class swaps_list : public swaps::list
        {
        public:
            inline explicit swaps_list() throw() : swaps::list() {}
            inline virtual ~swaps_list() throw() { }

            //! total list of counts for optimal partition
            inline size_t counts() const throw()
            {
                size_t sum = 0;
                for(const swaps *swp=head;swp;swp=swp->next)
                {
                    sum += swp->count;
                }
                return sum;
            }

            inline void __sort() const throw()
            {
                const swaps::list &_self = *this;
                swaps::list       &self  = (swaps::list &)_self;
                core::merging<swaps>::sort(self,swaps::compare_by_pos,NULL);
            }
            
            
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(swaps_list);
        };

        typedef addr_list<swaps> swaps_addr_list;
        typedef addr_node<swaps> swaps_addr_node;
    }
}
#endif
