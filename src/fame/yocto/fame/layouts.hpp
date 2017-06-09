#ifndef YOCTO_FAME_LAYOUTS_INCLUDED
#define YOCTO_FAME_LAYOUTS_INCLUDED 1

#include "yocto/fame/domain.hpp"
#include "yocto/fame/ghost.hpp"

namespace yocto
{

    namespace fame
    {

        template <typename COORD>
        class layouts : public domain<COORD>
        {
        public:
            YOCTO_FAME_DECL_COORD;
            typedef domain<COORD>              domain_type;
            typedef layout<COORD>              layout_type;
            typedef typename domain_type::link link;
            static const size_t NUM_INDICES = YOCTO_ROUND4(DIMENSION);


            const coord1d     depth; //!< ghost depths
            const layout_type outer; //!< outer layout, inner is from domain
            int               minRealIndices[NUM_INDICES];
            int               maxRealIndices[NUM_INDICES];
            
            inline virtual ~layouts() throw() {}

            inline explicit layouts(const domain_type &dom,
                                    const coord1d      num_ghosts) :
            domain_type(dom),
            depth(num_ghosts<0?0:num_ghosts),
            outer( __expand(this->inner,*this,depth) ),
            minRealIndices(),
            maxRealIndices()
            {
                memset(minRealIndices,0,sizeof(minRealIndices));
                memset(maxRealIndices,0,sizeof(maxRealIndices));
                this->compute_real_indices(outer,depth,minRealIndices,maxRealIndices);
            }
            
            inline layouts(const layouts &other) :
            domain_type(*other),
            depth(other.depth),
            outer(other.outer),
            minRealIndices(),
            maxRealIndices()
            {
                memcpy(minRealIndices,other.minRealIndices,sizeof(minRealIndices));
                memcpy(maxRealIndices,other.maxRealIndices,sizeof(maxRealIndices));
            }

        private:
            YOCTO_DISABLE_ASSIGN(layouts);
            static inline
            layout_type __expand(const layout_type &L,
                                 const domain_type &D,
                                 coord1d           ng)
            {
                assert(ng>=0);
                coord Lower = L.lower;
                coord Upper = L.upper;
                for(size_t dim=0;dim<DIMENSION;++dim)
                {
                    coord1d     &lower = __coord(Lower,dim);
                    coord1d     &upper = __coord(Upper,dim);
                    const link  &lnk   = D.links[dim];
                    if(lnk.prev)
                    {
                        lower -= ng;
                    }
                    if(lnk.next)
                    {
                        upper += ng;
                    }
                }

                return layout_type(Lower,Upper);
            }


        };
    }

}

#endif
