#include "yocto/ipso/divide.hpp"
#include "yocto/code/utils.hpp"

namespace yocto
{
    namespace ipso
    {
        template <>
        coord1D divider<coord1D>::optimal_for(const patch<coord1D> &p, const size_t max_cpu, coord1D *fallback)
        {
            const coord1D cpus = max_of<coord1D>(max_cpu,1);
            const coord1D nn   = min_of<coord1D>(cpus,p.width);
            if(fallback)
            {
                *fallback = nn;
            }
            return nn;
        }

        template <typename COORD>
        class subdivision : public object
        {
        public:
            typedef core::list_of_cpp<subdivision> list;
            const COORD                            sizes;
            const size_t                           items;
            subdivision                           *next;
            subdivision                           *prev;

            virtual ~subdivision() throw() {}
            explicit subdivision(const divider<COORD> &full) throw() :
            sizes(full.sizes),
            items(full.max_items_per_patch()),
            next(0),
            prev(0)
            {}


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(subdivision);
        };

        template <typename COORD>
        class subdivisions : public core::list_of_cpp< subdivision<COORD> >
        {
        public:
            typedef subdivision<COORD> sub_type;
            inline explicit subdivisions() throw() {}
            inline virtual ~subdivisions() throw() {}


            inline void show() const
            {
                for(const sub_type *sub = this->head; sub; sub=sub->next )
                {
                    std::cerr << "for " << sub->sizes << " : max_items=" << sub->items << std::endl;
                }
            }
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(subdivisions);
        };


        template <>
        coord2D divider<coord2D>::optimal_for(const patch<coord2D> &p, const size_t max_cpu, coord2D *fallback)
        {
            const coord1D cpus = max_of<coord1D>(max_cpu,1);
            const coord1D nx   = min_of<coord1D>(cpus,p.width.x);
            const coord1D ny   = min_of<coord1D>(cpus,p.width.x);
            subdivisions<coord2D>  subs;

            // build all possible subdivisions
            {
                coord2D sizes;
                for(sizes.x=1;sizes.x<=nx;++sizes.x)
                {
                    for(sizes.y=1;sizes.y<=ny;++sizes.y)
                    {
                        if(sizes.__prod()<=cpus)
                        {
                            const divide::in2D full(sizes,p);
                            subs.push_back( new subdivision<coord2D>(full) );
                        }
                    }
                }
            }
            subs.show();
            return coord2D(1,1);
        }
    }
}




