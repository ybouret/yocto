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
            const   size_t cores;
            const   COORD  offset;
            const   COORD  length;
            mutable COORD  last_ranks;

            inline virtual ~__split() throw() {}
            //! get local ranks from global rank
            virtual COORD        get_ranks(const size_t rank)  const throw() = 0;

            //! get sub patch from global rank
            virtual patch<COORD> operator()(const size_t rank) const throw() = 0;

        protected:
            inline explicit __split(const size_t        n,
                                    const patch<COORD> &p) throw() :
            cores(n),
            offset(p.lower),
            length(p.width),
            last_ranks()
            {
                assert(cores>0);
            }


            inline explicit __split(const __split &other) throw() :
            cores(other.cores),
            offset(other.offset),
            length(other.length),
            last_ranks(other.last_ranks)
            {
            }

        private:
            YOCTO_DISABLE_ASSIGN(__split);
        };


        struct split
        {

            static inline  bool is_bulk(const coord1D rank,
                                        const coord1D size) throw()
            {
                return (size>1) && (rank>0) && (rank<(size-1));
            }

            static inline bool is_side(const coord1D rank,
                                       const coord1D size) throw()
            {
                return !is_bulk(rank,size);
            }

            class in1D : public __split<coord1D>
            {
            public:
                typedef __split<coord1D> split_type;
                typedef patch1D          patch_type;


                inline explicit in1D(const coord1D &n,
                                     const patch1D &p) throw() :
                split_type(n,p)
                {
                    assert(length>=coord1D(cores));
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
                    last_ranks = rank;
                    return patch1D(__offset,__offset+__length);
                }

                virtual coord1D get_ranks(const size_t rank) const throw()
                {
                    return rank;
                }

                static coord1D computeCoresMap(const size_t cores, const coord1D length, const coord1D pbc) throw();


            private:
                YOCTO_DISABLE_ASSIGN(in1D);
            };

            class in2D : public __split<coord2D>
            {
            public:
                typedef __split<coord2D> split_type;
                typedef patch2D          patch_type;

                const coord2D ncore; //!< cores map

                inline virtual ~in2D() throw() {}

                inline explicit in2D(const coord2D  n,
                                     const patch2D &p) throw() :
                split_type(n.x*n.y,p),
                ncore(n)
                {
                }

                inline in2D(const in2D &other) throw() :
                split_type(other),
                ncore(other.ncore)
                {
                }

                inline coord2D get_ranks(const size_t rank) const throw()
                {
                    assert(rank<cores);
                    const ldiv_t l = ldiv( long(rank), long(ncore.x) );
                    assert(unit_t(l.rem)<ncore.x);
                    assert(unit_t(l.quot)<ncore.y);
                    return coord2D(l.rem,l.quot);
                }


                virtual patch_type operator()(const size_t rank) const throw()
                {
                    assert(rank<cores);
                    const coord2D ranks = get_ranks(rank);
                    coord2D __offset = offset;
                    coord2D __length = length;
                    basic_split(ranks.x,ncore.x,__offset.x,__length.x);
                    basic_split(ranks.y,ncore.y,__offset.y,__length.y);
                    __length += __offset;
                    __coord_dec(__length);

                    last_ranks = ranks;
                    return patch_type(__offset,__length);
                }

                static coord2D computeCoresMap(const size_t cores, const coord2D length, const coord2D pbc);


            private:
                YOCTO_DISABLE_ASSIGN(in2D);
            };


            class in3D : public __split<coord3D>
            {
            public:
                typedef __split<coord3D> split_type;
                typedef patch3D          patch_type;

                const coord3D ncore; //!< cores map

                inline virtual ~in3D() throw() {}

                inline explicit in3D(const coord3D  n,
                                     const patch3D &p) throw() :
                split_type(n.x*n.y*n.z,p),
                ncore(n)
                {
                }


                inline in3D( const in3D &other ) throw() :
                split_type(other),
                ncore(other.ncore)
                {
                }

                inline coord3D get_ranks(const size_t rank) const throw()
                {
                    assert(rank<cores);
                    const ldiv_t lx = ldiv( long(rank), long(ncore.x) );
                    const unit_t xr = unit_t(lx.rem);  assert(xr<ncore.x);
                    const ldiv_t ly = ldiv( lx.quot, long(ncore.y) );
                    const unit_t yr = unit_t(ly.rem);  assert(yr<ncore.y);
                    const unit_t zr = unit_t(ly.quot); assert(zr<ncore.z);
                    return coord3D(xr,yr,zr);
                }

                virtual patch_type operator()(const size_t rank) const throw()
                {
                    assert(rank<cores);
                    const coord3D ranks = get_ranks(rank);
                    coord3D __offset = offset;
                    coord3D __length = length;
                    
                    basic_split(ranks.x,ncore.x,__offset.x,__length.x);
                    basic_split(ranks.y,ncore.y,__offset.y,__length.y);
                    basic_split(ranks.z,ncore.z,__offset.z,__length.z);

                    __length += __offset;
                    __coord_dec(__length);

                    last_ranks = ranks;
                    return patch_type(__offset,__length);
                }

                static coord3D computeCoresMap(const size_t cores, const coord3D length, const coord3D pbc);


            private:
                YOCTO_DISABLE_ASSIGN(in3D);
            };

        };

    }
}

#endif
