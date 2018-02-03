#include "yocto/ipso/divide.hpp"
#include "yocto/code/utils.hpp"
#include "yocto/sort/merge.hpp"

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

            static inline int compare_sizes( const COORD &L, const COORD &R ) throw()
            {
                const coord1D Lnorm = __coord_sum(L);
                const coord1D Rnorm = __coord_sum(R);
                if(Lnorm<Rnorm)
                {
                    return -1;
                }
                else
                {
                    if(Rnorm<Lnorm)
                    {
                        return 1;
                    }
                    else
                    {
                        return __lexicographic_inc_fwd<coord1D,sizeof(COORD)/sizeof(coord1D)>(&L,&R);
                    }
                }
            }

            static inline int compare(const subdivision *lhs, const subdivision *rhs, void *) throw()
            {
                if(lhs->items<rhs->items)
                {
                    return -1;
                }
                else
                {
                    if(rhs->items<lhs->items)
                    {
                        return 1;
                    }
                    else
                    {
                        assert(lhs->items==rhs->items);
                        {
                            return compare_sizes(lhs->sizes,rhs->sizes);
                        }
                    }
                }

            }
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(subdivision);
        };

        template <typename COORD>
        class subdivisions : public core::list_of_cpp< subdivision<COORD> >
        {
        public:
            typedef subdivision<COORD>          sub_type;
            typedef core::list_of_cpp<sub_type> list_type;

            inline explicit subdivisions() throw() : list_type() {}
            inline virtual ~subdivisions() throw() {}


            inline void show() const
            {
                for(const sub_type *sub = this->head; sub; sub=sub->next )
                {
                    std::cerr << "\tfor " << sub->sizes << " : max_items=" << sub->items << std::endl;
                }
            }

            void sort()
            {
                list_type &self = *this;
                core::merging<sub_type>::sort(self,sub_type::compare,NULL);
            }

            inline COORD get_optimal( COORD *fallback ) throw()
            {
                assert(this->size>0);
                sort();
                std::cerr << "full" << std::endl;
                show();
                {
                    list_type stk;
                    stk.push_back(this->pop_front());
                    const coord1D num_opt = __coord_prod(stk.head->sizes);
                    while( this->size > 0 )
                    {
                        sub_type *sub = this->pop_front();
                        if( (__coord_prod(sub->sizes)>num_opt) && (stk.size<=1) )
                        {
                            stk.push_back(sub);
                        }
                        else
                        {
                            delete sub;
                        }
                    }
                    this->swap_with(stk);
                }
                assert(this->size>0);
                std::cerr << "kept" << std::endl;
                show();
                const COORD sz = this->head->sizes;
                const COORD fb = this->tail->sizes;
                if(fallback) *fallback = fb;
                return sz;
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(subdivisions);
        };


        template <>
        coord2D divider<coord2D>::optimal_for(const patch<coord2D> &p, const size_t max_cpu, coord2D *fallback)
        {
            const coord1D          cpus = max_of<coord1D>(max_cpu,1);
            const coord1D          nx   = min_of<coord1D>(cpus,p.width.x);
            const coord1D          ny   = min_of<coord1D>(cpus,p.width.y);
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
            assert(subs.size>0);
            return subs.get_optimal(fallback);
        }

        template <>
        coord3D divider<coord3D>::optimal_for(const patch<coord3D> &p, const size_t max_cpu, coord3D *fallback)
        {
            const coord1D          cpus = max_of<coord1D>(max_cpu,1);
            const coord1D          nx   = min_of<coord1D>(cpus,p.width.x);
            const coord1D          ny   = min_of<coord1D>(cpus,p.width.y);
            const coord1D          nz   = min_of<coord1D>(cpus,p.width.z);
            subdivisions<coord3D>  subs;

            // build all possible subdivisions
            {
                coord3D sizes;
                for(sizes.x=1;sizes.x<=nx;++sizes.x)
                {
                    for(sizes.y=1;sizes.y<=ny;++sizes.y)
                    {

                        for(sizes.z=1;sizes.z<=nz;++sizes.z)
                        {
                            if(sizes.__prod()<=cpus)
                            {
                                const divide::in3D full(sizes,p);
                                subs.push_back( new subdivision<coord3D>(full) );
                            }
                        }
                    }
                }
            }
            assert(subs.size>0);
            return subs.get_optimal(fallback);
        }
    }
}




