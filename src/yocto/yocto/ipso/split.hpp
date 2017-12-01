#ifndef YOCTO_IPSO_SPLIT_INCLUDED
#define YOCTO_IPSO_SPLIT_INCLUDED 1

#include "yocto/ipso/patch.hpp"
#include "yocto/ipso/utils.hpp"
#include <cstdlib>

namespace yocto
{
    namespace ipso
    {

        template <typename COORD>
        class __split
        {
        public:
            const size_t cores;
            const COORD  offset;
            const COORD  length;

            inline virtual ~__split() throw() {}
            inline virtual patch<COORD> operator()(const size_t rank) const throw() = 0;

        protected:
            inline explicit __split(const size_t        n,
                                    const patch<COORD> &p) throw() :
            cores(n),
            offset(p.lower),
            length(p.width)
            {
                assert(cores>0);
            }


            inline explicit __split(const __split &other) throw() :
            cores(other.cores),
            offset(other.offset),
            length(other.length)
            {
            }

        private:
            YOCTO_DISABLE_ASSIGN(__split);
        };


        struct split
        {



            class in1D : public __split<coord1D>
            {
            public:
                typedef __split<coord1D> split_type;
                typedef patch1D          patch_type;


                inline explicit in1D(const size_t   n,
                                     const patch1D &p) throw() :
                split_type(n,p)
                {
                    assert(length>=cores);
                }

                inline in1D(const in1D &other) throw() : split_type(other) {}


                virtual patch_type operator()(const size_t rank) const throw()
                {
                    assert(rank<cores);
                    coord1D __offset = offset;
                    coord1D __length = length;
                    basic_split(rank,cores,__offset,__length);
                    assert(__length>0);
                    --__length;
                    return patch1D(__offset,__offset+__length);
                }

            private:
                YOCTO_DISABLE_ASSIGN(in1D);
            };

            class in2D : public __split<coord2D>
            {
            public:
                typedef __split<coord2D> split_type;
                typedef patch2D          patch_type;

                const coord2D cmap; //!< cores map

                inline virtual ~in2D() throw() {}

                inline explicit in2D(const size_t   nx,
                                     const size_t   ny,
                                     const patch2D &p) throw() :
                split_type(nx*ny,p),
                cmap(nx,ny)
                {
                }

                inline in2D(const in2D &other) throw() :
                split_type(other),
                cmap(other.cmap)
                {
                }

                inline coord2D get_ranks(const size_t rank) const throw()
                {
                    assert(rank<cores);
                    const ldiv_t l = ldiv( long(rank), long(cmap.x) );
                    assert(unit_t(l.rem)<cmap.x);
                    assert(unit_t(l.quot)<cmap.y);
                    return coord2D(l.rem,l.quot);
                }


                virtual patch_type operator()(const size_t rank) const throw()
                {
                    assert(rank<cores);
                    const coord2D ranks = get_ranks(rank);
                    coord2D __offset = offset;
                    coord2D __length = length;
                    basic_split(ranks.x,cmap.x,__offset.x,__length.x);
                    basic_split(ranks.y,cmap.y,__offset.y,__length.y);
                    __length += __offset;
                    __coord_dec(__length);
                    return patch_type(__offset,__length);
                }


            private:
                YOCTO_DISABLE_ASSIGN(in2D);
            };


            class in3D : public __split<coord3D>
            {
            public:
                typedef __split<coord3D> split_type;
                typedef patch3D          patch_type;

                const coord3D cmap; //!< cores map
                //const coord1D coef; //!< nx*ny

                inline virtual ~in3D() throw() {}

                inline explicit in3D(const size_t   nx,
                                     const size_t   ny,
                                     const size_t   nz,
                                     const patch3D &p) throw() :
                split_type(nx*ny*nz,p),
                cmap(nx,ny,nz)
                {
                }


                inline in3D( const in3D &other ) throw() :
                split_type(other),
                cmap(other.cmap)
                {
                }

                inline coord3D get_ranks(const size_t rank) const throw()
                {
                    assert(rank<cores);
                    const ldiv_t lx = ldiv( long(rank), long(cmap.x) );
                    const unit_t xr = unit_t(lx.rem);  assert(xr<cmap.x);
                    const ldiv_t ly = ldiv( lx.quot, long(cmap.y) );
                    const unit_t yr = unit_t(ly.rem);  assert(yr<cmap.y);
                    const unit_t zr = unit_t(ly.quot); assert(zr<cmap.z);
                    return coord3D(xr,yr,zr);
                }

                virtual patch_type operator()(const size_t rank) const throw()
                {
                    assert(rank<cores);
                    const coord3D ranks = get_ranks(rank);
                    coord3D __offset = offset;
                    coord3D __length = length;
                    
                    basic_split(ranks.x,cmap.x,__offset.x,__length.x);
                    basic_split(ranks.y,cmap.y,__offset.y,__length.y);
                    basic_split(ranks.z,cmap.z,__offset.z,__length.z);

                    __length += __offset;
                    __coord_dec(__length);
                    return patch_type(__offset,__length);
                }

            private:
                YOCTO_DISABLE_ASSIGN(in3D);
            };

        };

    }
}

#endif

