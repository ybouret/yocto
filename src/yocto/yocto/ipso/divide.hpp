#ifndef YOCTO_IPSO_DIVIDE_INCLUDED
#define YOCTO_IPSO_DIVIDE_INCLUDED 1

#include "yocto/ipso/patch.hpp"
#include "yocto/ipso/utils.hpp"

#include "yocto/exceptions.hpp"

#include <cerrno>

namespace yocto
{
    namespace ipso
    {

        template <typename COORD>
        class divider : public patch<COORD>
        {
        public:
            const COORD   sizes;     //!< sizes in different dimension
            const size_t  size;      //!< total number of cores, MPI style
            const COORD   lasts;     //!< last ranks
            //! destructor
            inline virtual ~divider() throw() {}

            //! copy constructor
            inline divider(const divider &other) throw() :
            patch<COORD>(other),
            size(other.size),
            lasts(other.lasts)
            {
            }


            //! get local ranks from global rank
            virtual COORD get_ranks(const size_t rank)  const throw() = 0;

            //! get global rank from local ranks
            virtual size_t get_rank_from(const COORD ranks) const throw() = 0;

            //! get local patch from this...
            virtual patch<COORD> operator()(const size_t rank, COORD *pRanks) const throw() = 0;

            //! get the global rank of the previous local ranks in dimension 'dim'
            inline size_t prev_rank( COORD ranks, const size_t dim) const throw()
            {
                assert(dim<patch<COORD>::DIM);
                coord1D &r = __coord(ranks,dim);
                if(r<=0)
                {
                    r = __coord(lasts,dim);
                }
                else
                {
                    --r;
                }
                return get_rank_from(ranks);
            }

            //! get the global rank of the next local ranks in dimension 'dim'
            inline size_t next_rank( COORD ranks, const size_t dim) const throw()
            {
                assert(dim<patch<COORD>::DIM);
                coord1D &r = __coord(ranks,dim);
                if(r>=__coord(lasts,dim))
                {
                    r = 0;
                }
                else
                {
                    ++r;
                }
                return get_rank_from(ranks);
            }

        protected:
            explicit divider(const COORD         s,
                              const patch<COORD> &p) :
            patch<COORD>(p),
            sizes(s),
            size( __coord_prod(sizes) ),
            lasts(sizes)
            {
                if(size<=0) throw libc::exception(EDOM,"__divide<%uD>: no cores", unsigned(patch<COORD>::DIM));
                __coord_dec( (COORD &)lasts );
            }
            


        private:
            YOCTO_DISABLE_ASSIGN(divider);
        };




        struct divide
        {

            ////////////////////////////////////////////////////////////////////
            //
            //! 1D API
            //
            ////////////////////////////////////////////////////////////////////
            class in1D : public divider<coord1D>
            {
            public:
                typedef patch1D           patch_type;
                typedef divider<coord1D>  divider_type;


                inline explicit in1D(const coord1D     s,
                                     const patch_type &p) :
                divider_type(s,p)
                {
                    if(sizes>this->width)
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

                inline virtual size_t get_rank_from(const coord1D ranks) const throw()
                {
                    assert(ranks>=0);
                    assert(ranks<sizes);
                    return ranks;
                }

                inline virtual patch_type operator()(const size_t rank, coord1D *pRanks) const throw()
                {
                    coord1D lo = this->lower;
                    coord1D up = this->width;
                    if(pRanks) *pRanks = rank;
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
            class in2D : public divider<coord2D>
            {
            public:
                typedef patch2D           patch_type;
                typedef divider<coord2D>  divider_type;


                inline explicit in2D( const coord2D userSizes, const patch_type &p) :
                divider_type(userSizes,p)
                {
                    for(size_t dim=0;dim<DIM;++dim)
                    {
                        if( __coord(sizes,dim) > __coord(this->width,dim) )
                        {
                            throw libc::exception(EDOM,"in2D.%c: size exceeds witdh",char('x'+dim));
                        }
                    }
                }

                inline virtual ~in2D() throw()
                {
                }


                inline virtual coord2D get_ranks(const size_t rank)  const throw()
                {
                    assert(rank<size);
                    const ldiv_t l = ldiv( long(rank), long(sizes.x) );
                    assert(unit_t(l.rem)<sizes.x);
                    assert(unit_t(l.quot)<sizes.y);
                    return coord2D(l.rem,l.quot);
                }

                inline virtual size_t get_rank_from(const coord2D ranks) const throw()
                {
                    assert(ranks.x>=0);
                    assert(ranks.x<sizes.x);
                    assert(ranks.y>=0);
                    assert(ranks.y<sizes.y);

                    return ranks.y * sizes.x + ranks.x;
                }


                inline virtual patch_type operator()(const size_t rank, coord2D *pRanks) const throw()
                {
                    coord2D       lo    = this->lower;
                    coord2D       up    = this->width;
                    const coord2D ranks = get_ranks(rank);
                    if(pRanks) *pRanks  = ranks;
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
            class in3D : public divider<coord3D>
            {
            public:
                typedef patch3D           patch_type;
                typedef divider<coord3D>  divider_type;


                inline explicit in3D(const coord3D     userSizes,
                                     const patch_type &p) throw() :
                divider_type(userSizes,p)
                {
                    for(size_t dim=0;dim<DIM;++dim)
                    {
                        if( __coord(sizes,dim) > __coord(this->width,dim) )
                        {
                            throw libc::exception(EDOM,"in3D.%c: size exceeds witdh",char('x'+dim));
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
                    return coord3D(xr,yr,zr);
                }

                inline virtual size_t get_rank_from(const coord3D ranks) const throw()
                {
                    assert(ranks.x>=0);
                    assert(ranks.x<sizes.x);
                    assert(ranks.y>=0);
                    assert(ranks.y<sizes.y);
                    assert(ranks.z>=0);
                    assert(ranks.z<sizes.z);

                    return (ranks.z * sizes.y + ranks.y) * sizes.x + ranks.x;
                }

                inline virtual patch_type operator()(const size_t rank, coord3D *pRanks) const throw()
                {
                    coord3D       lo    = this->lower;
                    coord3D       up    = this->width;
                    const coord3D ranks = get_ranks(rank);
                    if(pRanks) *pRanks  = ranks;
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

