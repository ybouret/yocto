#ifndef YOCTO_IPSO_DIVIDE_INCLUDED
#define YOCTO_IPSO_DIVIDE_INCLUDED 1

#include "yocto/ipso/patch.hpp"
#include "yocto/ipso/utils.hpp"

#include "yocto/exceptions.hpp"
#include "yocto/mpl/natural.hpp"

#include <cerrno>

namespace yocto
{
    namespace ipso
    {

        template <typename COORD>
        class __divide : public patch<COORD>
        {
        public:
            const size_t size; //!< total number of cores, MPI style

            //! destructor
            inline virtual ~__divide() throw() {}

            //! copy constructor
            inline __divide(const __divide &other) throw() :
            patch<COORD>(other),
            size(other.size)
            {
            }


            //! get local ranks from global rank
            virtual COORD get_ranks(const size_t rank)  const throw() = 0;

            //! get local patch from this...
            virtual patch<COORD> operator()(const size_t rank) const throw() = 0;

        protected:
            explicit __divide(const size_t        n,
                              const patch<COORD> &p) :
            patch<COORD>(p),
            size(n)
            {
                if(size<=0) throw libc::exception(EDOM,"__divide<%uD>: no cores", unsigned(patch<COORD>::DIM));
            }
            


        private:
            YOCTO_DISABLE_ASSIGN(__divide);
        };




        struct divide
        {

            static inline coord1D getBorder(const coord1D) throw()
            {
                return 1;
            }

            static inline coord2D getBorder(const coord2D L) throw()
            {
                return coord2D(L.y,L.x);
            }

            static inline coord3D getBorder(const coord3D L) throw()
            {
                return coord3D(L.y*L.z,L.x*L.z,L.x*L.y);
            }


            static inline bool isBulk(const size_t rank, const size_t size) throw()
            {
                return (size>1) && (rank>0) && (rank<size-1);
            }

            static inline bool isSide(const size_t rank, const size_t size) throw()
            {
                return !isBulk(rank,size);
            }

            template <typename COORD>
            class metrics
            {
            public:
                inline explicit metrics(const patch<COORD> &p,
                                        const COORD         ranks,
                                        const COORD         sizes,
                                        const COORD         pbc) :
                load(p.items),
                copy(0),
                coms(0)
                {
                    mpn to_copy = 0;
                    mpn to_coms = 0;
                    const COORD borders = getBorder(p.width);
                    for(size_t dim=0;dim<patch<COORD>::DIM;++dim)
                    {
                        const size_t rank     = __coord(ranks,dim);
                        const size_t size     = __coord(sizes,dim);
                        const bool   periodic = (0!=__coord(pbc,dim) );
                        const bool   bulk     = isBulk(rank,size);
                        const size_t border   = __coord(borders,dim);
                        assert(size>0);
                        assert(rank<size);
                    }

                    to_copy.xch( (mpn&)copy );
                    to_coms.xch( (mpn&)coms );
                }

                inline virtual ~metrics() throw()
                {
                }

                const mpn load;  //!< number of items to compute
                const mpn copy;  //!< number of items to copy
                const mpn coms;  //!< number of items to exchange via coms channels

                inline metrics(const metrics &other) :
                load(other.load),
                copy(other.copy),
                coms(other.coms)
                {
                }
                

            private:
                YOCTO_DISABLE_ASSIGN(metrics);
            };


            ////////////////////////////////////////////////////////////////////
            //
            //! 1D API
            //
            ////////////////////////////////////////////////////////////////////
            class in1D : public __divide<coord1D>
            {
            public:
                typedef patch1D           patch_type;
                typedef __divide<coord1D> divide_type;

                inline explicit in1D(const size_t      n,
                                     const patch_type &p) :
                divide_type(n,p)
                {
                    if( coord1D(size) >this->width)
                    {
                        throw libc::exception(EDOM,"in1D: size exceeds width");
                    }
                }

                inline virtual ~in1D() throw()
                {
                }


                inline virtual coord1D get_ranks(const size_t rank)  const throw()
                {
                    assert(rank<size);
                    return rank;
                }

                inline virtual patch_type operator()(const size_t rank) const throw()
                {
                    coord1D lo = this->lower;
                    coord1D up = this->width;
                    basic_split(rank,size,lo,up); assert(up>0);
                    up += lo;
                    --up;
                    return patch1D(lo,up);
                }

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(in1D);
            };


            ////////////////////////////////////////////////////////////////////
            //
            //! 2D API
            //
            ////////////////////////////////////////////////////////////////////
            class in2D : public __divide<coord2D>
            {
            public:
                typedef patch2D           patch_type;
                typedef __divide<coord2D> divide_type;

                const coord2D   sizes;
                mutable coord2D lastRanks;

                inline explicit in2D( const coord2D userSizes, const patch_type &p) throw() :
                divide_type(userSizes.__prod(),p),
                sizes(userSizes),
                lastRanks()
                {
                    for(size_t dim=0;dim<DIM;++dim)
                    {
                        if( __coord(sizes,dim) > __coord(this->width,dim) )
                        {
                            throw libc::exception(EDOM,"in2D.%c: size exceeds witdh",char('x'+DIM));
                        }
                    }
                }

                inline virtual ~in2D() throw()
                {
                }


                inline virtual coord2D get_ranks(const size_t rank)  const throw()
                {
                    assert(rank<size);
                    assert(rank<size);
                    const ldiv_t l = ldiv( long(rank), long(sizes.x) );
                    assert(unit_t(l.rem)<sizes.x);
                    assert(unit_t(l.quot)<sizes.y);
                    return (lastRanks=coord2D(l.rem,l.quot));
                }

                inline virtual patch_type operator()(const size_t rank) const throw()
                {
                    coord2D       lo    = this->lower;
                    coord2D       up    = this->width;
                    const coord2D ranks = get_ranks(rank);
                    basic_split(ranks.x,sizes.x,lo.x,up.x); assert(up.x>0);
                    basic_split(ranks.y,sizes.y,lo.y,up.y); assert(up.y>0);
                    up += lo;
                    __coord_dec(up);
                    return patch2D(lo,up);
                }

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(in2D);
            };

            ////////////////////////////////////////////////////////////////////
            //
            //! 3D API
            //
            ////////////////////////////////////////////////////////////////////
            class in3D : public __divide<coord3D>
            {
            public:
                typedef patch3D           patch_type;
                typedef __divide<coord3D> divide_type;

                const coord3D   sizes;
                mutable coord3D lastRanks;

                inline explicit in3D(const coord3D     userSizes,
                                     const patch_type &p) throw() :
                divide_type(userSizes.__prod(),p),
                sizes(userSizes)
                {
                    for(size_t dim=0;dim<DIM;++dim)
                    {
                        if( __coord(sizes,dim) > __coord(this->width,dim) )
                        {
                            throw libc::exception(EDOM,"in3D.%c: size exceeds witdh",char('x'+DIM));
                        }
                    }
                }

                inline virtual ~in3D() throw()
                {
                }


                inline virtual coord3D get_ranks(const size_t rank)  const throw()
                {
                    assert(rank<size);
                    const ldiv_t lx = ldiv( long(rank), long(sizes.x) );
                    const unit_t xr = unit_t(lx.rem);  assert(xr<sizes.x);
                    const ldiv_t ly = ldiv( lx.quot, long(sizes.y) );
                    const unit_t yr = unit_t(ly.rem);  assert(yr<sizes.y);
                    const unit_t zr = unit_t(ly.quot); assert(zr<sizes.z);
                    return (lastRanks=coord3D(xr,yr,zr));
                }

                inline virtual patch_type operator()(const size_t rank) const throw()
                {
                    coord3D       lo    = this->lower;
                    coord3D       up    = this->width;
                    const coord3D ranks = get_ranks(rank);
                    basic_split(ranks.x,sizes.x,lo.x,up.x); assert(up.x>0);
                    basic_split(ranks.y,sizes.y,lo.y,up.y); assert(up.y>0);
                    basic_split(ranks.z,sizes.z,lo.z,up.z); assert(up.y>0);
                    up += lo;
                    __coord_dec(up);
                    return patch3D(lo,up);
                }

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(in3D);
            };



        };

    }
}

#endif

