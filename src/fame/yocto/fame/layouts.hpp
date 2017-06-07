#ifndef YOCTO_FAME_LAYOUTS_INCLUDED
#define YOCTO_FAME_LAYOUTS_INCLUDED 1

#include "yocto/fame/domain.hpp"
#include "yocto/fame/ghost.hpp"

namespace yocto
{
    
    namespace fame
    {
        template <typename COORD>
        class layouts_of : public domain_of<COORD>
        {
        public:
            YOCTO_FAME_DECL_COORD;
            typedef domain_of<COORD>           domain_type;
            typedef layout_of<COORD>           layout_type;
            typedef typename domain_type::link link;
            
            const coord1d     depth; //!< ghost depths
            const layout_type outer; //!< outer layouts
            inline virtual ~layouts_of() throw() {}
            
            inline explicit layouts_of(const domain_type &dom,
                                       const coord1d      num_ghosts) :
            domain_type(dom),
            depth(num_ghosts<0?0:num_ghosts),
            outer( __expand(this->inner,*this,depth) )
            {
                //std::cerr << "[new layout with depth=" << depth << "]" << std::endl;
            }
            
            
            
        private:
            YOCTO_DISABLE_ASSIGN(layouts_of);
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
