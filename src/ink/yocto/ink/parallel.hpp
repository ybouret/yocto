#ifndef YOCTO_INK_PARALLEL_INCLUDED
#define YOCTO_INK_PARALLEL_INCLUDED 1

#include "yocto/ink/bitmap.hpp"
#include "yocto/ipso/utils.hpp"
#include "yocto/math/point2d.hpp"

namespace yocto
{
    namespace Ink
    {
        typedef point2d<unit_t> coord;
        
        class Splitter : public Rectangle
        {
        public:
            const coord   sizes;     //!< sizes in different dimension
            const size_t  size;      //!< total number of cores, MPI style

            explicit Splitter(const coord user_sizes, const Rectangle &rect);
            virtual ~Splitter() throw();
            
            coord      getRanks(const size_t rank) const throw();
            size_t     getRank(const coord  ranks) const throw();
            Rectangle  operator()(const size_t rank, coord *pRanks) const throw();
            
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Splitter);
        };
        
        
        
    }
}

#endif
