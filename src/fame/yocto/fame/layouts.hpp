#ifndef YOCTO_FAME_LAYOUTS_INCLUDED
#define YOCTO_FAME_LAYOUTS_INCLUDED 1

#include "yocto/fame/domain.hpp"
#include "yocto/fame/ghost.hpp"

namespace yocto
{

    namespace fame
    {

        class real_indices
        {
        public:
            mutable int imin[4];
            mutable int imax[4];

            inline explicit real_indices() throw() : imin(), imax() { ldz(); }
            inline virtual ~real_indices() throw() {ldz();}
            inline real_indices(const real_indices &other) throw() : imin(), imax() { cpy(other); }
            inline real_indices & operator=(const real_indices &other) throw()
            {
                cpy(other);
                return *this;
            }

            inline void ldz() throw()
            {
                memset(imin,0,sizeof(imin));
                memset(imax,0,sizeof(imax));
            }

            inline void cpy(const real_indices &other) throw()
            {
                memmove(imin,other.imin,sizeof(imin));
                memmove(imax,other.imax,sizeof(imax));
            }
        };

        template <typename COORD>
        class layouts : public domain<COORD>, public real_indices
        {
        public:
            YOCTO_FAME_DECL_COORD;
            typedef domain<COORD>              domain_type;
            typedef layout<COORD>              layout_type;
            typedef typename domain_type::link link;
            static const size_t NUM_INDICES = YOCTO_ROUND4(DIMENSION);


            const coord1d     depth; //!< ghost depths
            const layout_type outer; //!< outer layout, inner is from domain

            inline virtual ~layouts() throw() {}

            inline explicit layouts(const domain_type &dom,
                                    const coord1d      num_ghosts) :
            domain_type(dom),
            real_indices(),
            depth(num_ghosts<0?0:num_ghosts),
            outer( __expand(this->inner,*this,depth) )
            {
                this->compute_real_indices(outer,depth,imin,imax);
            }
            
            inline layouts(const layouts &other) :
            domain_type(other),
            real_indices(other),
            depth(other.depth),
            outer(other.outer)
            {
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
