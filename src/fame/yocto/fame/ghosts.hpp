#ifndef YOCTO_FAME_GHOSTS_INCLUDED
#define YOCTO_FAME_GHOSTS_INCLUDED 1

#include "yocto/fame/layout.hpp"
#include "yocto/sequence/slots.hpp"
#include "yocto/sequence/some-arrays.hpp"
#include "yocto/code/bmove.hpp"

namespace yocto
{
    namespace fame
    {

        template <typename COORD> class layouts; //!< forward declaration
        class field_data;

        typedef array<coord1d>                        ghost_type;    //!< an array of indices
        typedef some_arrays<2,coord1d,memory::global> ghosts_type;   //!< 2 arrays of indices

        //______________________________________________________________________
        //
        //! a pair of ghosts on one side, one dimension
        //______________________________________________________________________
        class ghosts_pair : public ghosts_type
        {
        public:
            virtual ~ghosts_pair() throw();
            explicit ghosts_pair(const coord1d r,
                                 const size_t  n);

            const coord1d rank;  //!< would be MPI rank
            ghost_type   &outer; //!< to be received
            ghost_type   &inner; //!< to be sent

            //__________________________________________________________________
            //
            // FIELD base API
            //__________________________________________________________________


            //! load inner content in to memory, to be sent
            template <typename FIELD>
            inline void load( const FIELD &F, uint8_t * &p ) const throw()
            {
                const ghost_type &g = inner;
                for(size_t i=size;i>0;--i)
                {
                    const void *ptr = & F.entry[ g[i] ];
                    core::bmove< sizeof( typename FIELD::type) >(p,ptr);
                    p += sizeof( typename FIELD::type );
                }
            }

            //! save (received) memory into outer content
            template <typename FIELD>
            inline void save( FIELD &F, const uint8_t * &p ) const throw()
            {
                const ghost_type &g = outer;
                for(size_t i=size;i>0;--i)
                {
                    void *ptr = & F.entry[ g[i] ];
                    core::bmove< sizeof( typename FIELD::type) >(ptr,p);
                    p += sizeof( typename FIELD::type);
                }
            }

            //! local exchange
            template <typename FIELD> static inline
            void exchange( FIELD &F, const ghosts_pair &lhs, const ghosts_pair &rhs)
            {
                assert(lhs.size==rhs.size);
                for(size_t i=lhs.size;i>0;--i)
                {
                    F.entry[ lhs.outer[i] ] = F.entry[ rhs.inner[i] ];
                    F.entry[ rhs.outer[i] ] = F.entry[ lhs.inner[i] ];
                }
            }

            //! load inner content into memory, to be sent
            void stream_load( const field_data &F, uint8_t * &p ) const throw();

            //! save (received) memory into outer content
            void stream_save( field_data &F, const uint8_t * &p ) const throw();


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(ghosts_pair);
        };


        //______________________________________________________________________
        //
        //! two ghosts_pair, one dimension
        //______________________________________________________________________
        class ghosts
        {
        public:
            enum status
            {
                empty,
                async,
                lcopy
            };
            static const int has_none = 0x00;
            static const int has_prev = 0x01;
            static const int has_next = 0x02;
            static const int has_both = has_prev|has_next;

            ghosts() throw();
            ~ghosts() throw();

            ghosts_pair *prev;
            ghosts_pair *next;
            status       kind;
            int          flag;
            const char  *kind_text() const throw();
            size_t       size() const throw(); //!< common size
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(ghosts);
            void cleanup() throw();
        };



        //______________________________________________________________________
        //
        //! a base class to store YOCTO_FAME_DIM_OF(COORD) ghosts
        //______________________________________________________________________
        template <class COORD>
        class ghosts_base : public slots_of<ghosts>
        {
        public:
            YOCTO_FAME_DECL_COORD;

            inline virtual ~ghosts_base() throw() {}
            inline explicit ghosts_base(const coord1d r) :
            slots_of<ghosts>(DIMENSION),
            rank(r),
            num_async(0),
            num_lcopy(0),
            num_empty(0),
            async(),
            lcopy(),
            empty()
            {
                assert(rank>=0);
                for(size_t dim=0;dim<DIMENSION;++dim)
                {
                    (void)push_back();
                    async[dim] = 0;
                    lcopy[dim] = 0;
                    empty[dim] = 0;
                }
            }

            const coord1d rank;      //!< self rank
            const size_t  num_async; //!< should use MPI
            const size_t  num_lcopy; //!< should use lcopy
            const size_t  num_empty; //!< would do nothing
            ghosts *      async[DIMENSION];
            ghosts *      lcopy[DIMENSION];
            ghosts *      empty[DIMENSION];

            inline void collect_exchange_info() throw()
            {
                assert(0==num_async);
                assert(0==num_lcopy);
                size_t & na = (size_t&)num_async;
                size_t & nl = (size_t&)num_lcopy;
                size_t & ne = (size_t&)num_empty;
                
                for(size_t dim=0;dim<DIMENSION;++dim)
                {
                    ghosts *g    = & (*this)[dim];
                    int    &flag = g->flag; assert(ghosts::has_none==flag);
                    if(g->prev!=0)  { flag |= ghosts::has_prev; }
                    if(g->next!=0)  { flag |= ghosts::has_next; }

                    switch(flag)
                    {
                        case ghosts::has_none:
                            g->kind     = ghosts::empty;
                            empty[ne++] = g;
                            break;

                        case ghosts::has_prev:
                            g->kind     = ghosts::async;
                            async[na++] = g;
                            break;

                        case ghosts::has_next:
                            g->kind     = ghosts::async;
                            async[na++] = g;
                            break;

                        case ghosts::has_both:
                            assert(g->prev->size==g->next->size);
                            if( (g->prev->rank==rank) && (g->next->rank==rank) )
                            {
                                g->kind     = ghosts::lcopy;
                                lcopy[nl++] = g;
                            }
                            else
                            {
                                assert( (g->prev->rank!=rank) && (g->next->rank!=rank) );
                                g->kind = ghosts::async;
                                async[na++] = g;
                            }
                            break;

                    }
                }
                assert(DIMENSION==num_empty+num_async+num_lcopy);
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(ghosts_base);
        };

        //______________________________________________________________________
        //
        //! implementation of ghosts, compute indices from outer layout
        //______________________________________________________________________
        template <class COORD>
        class ghosts_of : public ghosts_base<COORD>
        {
        public:
            inline virtual ~ghosts_of() throw() {}
            explicit ghosts_of(const layouts<COORD> &L);



        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(ghosts_of);
        };

    }

}

#endif
