#ifndef YOCTO_IPSO_DIVIDE_INCLUDED
#define YOCTO_IPSO_DIVIDE_INCLUDED 1

#include "yocto/ipso/patch.hpp"
#include "yocto/exceptions.hpp"
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
                    if(size>this->width) throw libc::exception(EDOM,"in1D: size exceeds width");
                }

                inline virtual ~in1D() throw()
                {
                }


                inline virtual coord1D get_ranks(const size_t rank)  const throw()
                {
                    assert(rank<size);
                    return rank;
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

                const coord2D sizes;

                inline explicit in2D( const coord2D userSizes, const patch_type &p) throw() :
                divide_type(userSizes.__prod(),p),
                sizes(userSizes)
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
                    return coord2D(l.rem,l.quot);
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

                const coord3D sizes;

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
                    return coord3D(xr,yr,zr);
                }

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(in3D);
            };



        };

    }
}

#endif

