#ifndef YOCTO_FAME_LAYOUTS_INCLUDED
#define YOCTO_FAME_LAYOUTS_INCLUDED 1

#include "yocto/fame/domain.hpp"

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
            
            const layout_type outer;
            
            inline virtual ~layouts_of() throw() {}
            
            inline layouts_of(const domain_type &dom,
                              const coord1d      num_ghosts) :
            domain_type(dom),
            outer( __expand(this->inner,*this,num_ghosts) )
            {
            }
            
            
        private:
            YOCTO_DISABLE_ASSIGN(layouts_of);
            static inline
            layout_type __expand(const layout_type &L,
                                 const domain_type &D,
                                 coord1d           num_ghosts)
            {
                if(num_ghosts<0) num_ghosts=0;
                coord Lower = L.lower;
                coord Upper = L.upper;
                for(size_t dim=0;dim<DIMENSION;++dim)
                {
                    coord1d     &lower = __coord(Lower,dim);
                    coord1d     &upper = __coord(Upper,dim);
                    const link  &lnk   = D.links[dim];
                    if(lnk.prev)
                    {
                        lower -= num_ghosts;
                    }
                    if(lnk.next)
                    {
                        upper += num_ghosts;
                    }
                }
                
                return layout_type(Lower,Upper);
            }
        };
    }
    
}

#endif
