#ifndef YOCTO_INK_PARALLEL_INCLUDED
#define YOCTO_INK_PARALLEL_INCLUDED 1

#include "yocto/ink/bitmap.hpp"
#include "yocto/ipso/utils.hpp"
#include "yocto/math/point2d.hpp"
#include "yocto/core/list.hpp"

namespace yocto
{
    namespace Ink
    {
        typedef point2d<unit_t> coord;

        class Domain : public Rectangle
        {
        public:
            typedef core::list_of_cpp<Domain> List;

            const coord  ranks;
            const size_t items;
            Domain      *next;
            Domain      *prev;
            
            explicit Domain(const Rectangle &user_rect, const coord &user_ranks);
            virtual ~Domain() throw();

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Domain);
        };


        class Partition : public Rectangle
        {
        public:
            typedef core::list_of_cpp<Partition> List;

            const coord   sizes;     //!< sizes in different dimension
            const size_t  cores;     //!< total number of cores, MPI style
            const size_t  score;     //!< maximum size of sub rectangles
            Partition    *next;
            Partition    *prev;
            
            explicit Partition(const coord user_sizes, const Rectangle &rect) throw();
            virtual ~Partition() throw();
            
            coord      getRanks(const size_t rank) const throw();
            size_t     getRank(const coord  ranks) const throw();
            Rectangle  operator()(const size_t rank, coord *pRanks) const throw();

            void          computeScore( Domain::List *domains ) const;
            static void   Build(List            &parts,
                                const Rectangle &full,
                                const size_t     max_cpus );

            static coord  Optimal(const Rectangle &full, const size_t max_cpus);

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Partition);
        };
        
        

        
    }
}

#endif
