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

            const coord1d     depth; //!< ghost depths
            const layout_type outer; //!< outer layout, inner is from domain
            inline virtual ~layouts() throw() {}

            inline explicit layouts(const domain_type &dom,
                                    const coord1d      num_ghosts) :
            domain_type(dom),
            depth(num_ghosts<0?0:num_ghosts),
            outer( __expand(this->inner,*this,depth) )
            {
            }

            inline void compute_real_indices_on(const size_t dim,
                                                int         &idxMin,
                                                int         &idxMax)
            {
                assert(depth>0);
                idxMin = 0;
                idxMax = __coord(outer.upper,dim)-1;
                std::cerr << "computing real indices..." << std::endl;
                const coord1d dim_rank = __coord(this->self.ranks,dim);
                //const coord1d dim_size = __coord(this->full.sizes,dim);
                const coord1d dim_rmax = __coord(this->full.rmax,dim);
                std::cerr << "dim_rank=" << dim_rank << ", dim_rmax=" << dim_rmax << std::endl;
                if(dim_rank<=0)
                {
                    // no ghost on lower bound
                    idxMin += depth;
                }
                else
                {
                    if(dim_rank>=dim_rmax)
                    {
                        // no ghost on upper bound
                        idxMax -= depth;
                    }
                    else
                    {
                        // 'in the middle'

                    }
                }
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
