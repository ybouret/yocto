#ifndef YOCTO_IPSO_SWAPS_INCLUDED
#define YOCTO_IPSO_SWAPS_INCLUDED 1

#include "yocto/ipso/patch.hpp"
#include "yocto/ipso/swap-buffers.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/counted-object.hpp"
#include "yocto/ptr/intr.hpp"
#include "yocto/associative/set.hpp"

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
            
            const swap           recv;      //!< outer indices
            const swap           send;      //!< inner indices
            const size_t         count;     //!< send.size() = recv.size() = count
            mutable swap_buffers iobuf;     //!< local memory for send and recv
            const size_t         source;    //!< rank of source
            const size_t         target;    //!< rank of target
            const coord1D        layers;    //!< number of extra layers
            const bool           joined;    //!< flag for sanity
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

            explicit swaps(const swaps &lhs, const swaps &rhs);

            bool built() const throw();

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(swaps);
            void allocate();
            void io_check() throw();
        };

#define YOCTO_IPSO_SWAP_LOAD()  \
allocate();                     \
swap    & _recv = (swap &)recv; \
swap    & _send = (swap &)send;

        class swaps_list : public swaps::list
        {
        public:
            inline explicit swaps_list() throw() : swaps::list() {}
            inline virtual ~swaps_list() throw() {}

            //! total list of counts for optimal partition
            inline size_t   counts() const throw()
            {
                size_t sum = 0;
                for(const swaps *swp=head;swp;swp=swp->next)
                {
                    sum += swp->count;
                }
                return sum;
            }

            void join()
            {
                std::cerr << " ** JOINING ** " << std::endl;
                if(size<=0) return;
                swaps::list tmp;
                tmp.push_back( pop_front() );
                while(size>0)
                {
                    swaps *swp = pop_front();
                    if(swp->target==tmp.tail->target)
                    {
                        std::cerr << "\tshould join " << swaps::pos2txt(swp->pos) << " and " << swaps::pos2txt(tmp.tail->pos) << std::endl;
                        swaps *j = new swaps(*(tmp.tail),*swp);
                        delete swp;
                        delete tmp.pop_back();
                        tmp.push_back(j);
                    }
                    else
                    {
                        tmp.push_back(swp);
                    }
                }

                swap_with(tmp);
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(swaps_list);
        };

#if 0
        //! a node with a handle on swaps
        class meta_swaps : public object
        {
        public:
            typedef core::list_of_cpp<meta_swaps> list;
            const swaps * const handle;
            meta_swaps  *       next;
            meta_swaps  *       prev;

            explicit meta_swaps( const swaps *swp ) throw() :handle(swp), next(0), prev(0) {}
            virtual ~meta_swaps() throw() {}
            YOCTO_DISABLE_COPY_AND_ASSIGN(meta_swaps);
        };

        //! a list of swaps with the same target, and obviously the same source
        class meta_swaps_list : public counted_object, public meta_swaps::list
        {
        public:
            typedef intr_ptr<size_t,meta_swaps_list> pointer;
            typedef set<size_t,pointer>              table;

            const size_t target;
            explicit meta_swaps_list(const size_t tgt) throw() : target(tgt) {}
            virtual ~meta_swaps_list() throw() {}

            const size_t & key() const throw() { return target; }

            bool owns( const swaps *swp)
            {
                for(const meta_swaps *ms = this->head;ms;ms=ms->next)
                {
                    if(swp==ms->handle) return true;
                }
                return false;
            }

            void add( const swaps *swp )
            {
                assert(swp); assert(target==swp->target); assert(!owns(swp));
                push_back( new meta_swaps(swp) );
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(meta_swaps_list);
        };

        //! a table of meta_swaps_list, to group swaps by targets
        class swaps_table : public meta_swaps_list::table
        {
        public:
            explicit swaps_table() throw() : meta_swaps_list::table() {}
            virtual ~swaps_table() throw() {}

            void add( const swaps *swp )
            {
                assert(swp);
                const size_t tgt = swp->target;
                meta_swaps_list::pointer *ppL = search(tgt);
                if(ppL)
                {
                    (*ppL)->add(swp);
                }
                else
                {
                    meta_swaps_list *pL = new meta_swaps_list(tgt);
                    const meta_swaps_list::pointer q(pL);
                    if(!insert(q)) throw exception("swaps_table unexpected failure");
                    pL->add(swp);
                }
            }


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(swaps_table);
        };
#endif
        
    }
}
#endif
