#ifndef YOCTO_THREADING_CONTEXT_INCLUDED
#define YOCTO_THREADING_CONTEXT_INCLUDED 1

#include "yocto/parallel/basic.hpp"
#include "yocto/container/vslot.hpp"
#include "yocto/container/tuple.hpp"
#include "yocto/threading/threads.hpp"
#include "yocto/functor.hpp"

namespace yocto
{
    namespace threading
    {
        //! a context = range information for a working thread
        class context : public vslot
        {
        public:
            context(const size_t    the_rank,
                    const size_t    the_size,
                    lockable       &the_lock) throw();
            virtual ~context() throw();

            const size_t rank;   //!< 0..size-1
            const size_t size;   //!< how many contextes
            lockable    &access; //!< shared access
            const size_t indx;   //!< 1..size
            //vslot        data;   //!< for thread specific data

            //! splitting/using data
            /**
             - code example
             size_t offset = 1;
             size_t length = idata.size();
             ctx.split(offset, length);
             for(size_t i=offset,count=length;count-->0;++i)
             {
             // do something...
             }
             - or
             #define DO_SOMETHING(I) process(I)
             YOCTO_LOOP_FUNC(length,DO_SOMETHING,offset);
             */

            template <typename T> inline
            void split(T &offset,T &length) const throw()
            {
                parallel::basic_split<T>(rank, size, offset, length);
            }

            //! example of data
            YOCTO_PAIR_DECL(YOCTO_TUPLE_STANDARD,range,const size_t,offset,const size_t,length);
            inline range(const context &ctx,const size_t n) throw() :
            offset(1), length(n) { ctx.split<size_t>((size_t&)offset,(size_t&)length); }
            YOCTO_PAIR_END();

            //! example of data creation
            inline void create_range(const size_t n)
            {
                build<range,const context &,size_t>(*this,n);
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(context);
            friend class crew;
            void        *priv; //!< used by crew setup
        };

        template<typename ARRAY>
        YOCTO_TRIPLE_DECL(YOCTO_TUPLE_TEMPLATE,processor_of,const size_t,offset,const size_t,length,ARRAY,arr);
        inline processor_of(const context &ctx,ARRAY &a) throw() :
        offset(1),
        length(a.size()),
        arr(a)
        {
            ctx.split<size_t>((size_t&)offset,(size_t&)length);
        }

        YOCTO_TRIPLE_END();

        //! a threading kernel, using a context to know which data process
        typedef functor<void,TL1(context&)> kernel;

    }
}

#endif
