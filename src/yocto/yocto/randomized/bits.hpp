#ifndef YOCTO_RANDOMIZED_BITS_INCLUDED
#define YOCTO_RANDOMIZED_BITS_INCLUDED 1

#include "yocto/memory/buffer.hpp"
#include "yocto/code/bswap.hpp"
#include <math.h>

namespace yocto
{
    namespace Randomized
    {
        //______________________________________________________________________
        //
        //! 32 bits uniform generator interface
        //______________________________________________________________________
        class Bits : public object
        {
        public:
            const uint32_t span;     //!< returns in 0..span
            const size_t   bits;     //!< bits_for_span
            const uint32_t half;     //!< span/2
            const double   denD;     //!< double(span)+1.0
            const float    denF;     //!< float(span)+1.0f
            const double   halfDenD; //!< denD/2
            const float    halfDenF; //!< denF/2
            const double   addNumD;  //!< 0.5-halfDenD
            const float    addNumF;  //!< 0.5f-halfDenF
            virtual ~Bits() throw();

            virtual uint32_t next32() throw() = 0;
            virtual void     reseed( Bits &bits ) throw() = 0;

            template <typename T> T to()   throw(); //!< in 0:1 exclusive
            template <typename T> T symm() throw(); //!< in -1:1 exclusive

            //! random bit as integral type
            template <typename T> inline
            T nextBit() throw()
            {
                return ( ( next32() < half ) ? T(0) : T(1) );
            }

            inline bool nextBool() throw() { return ( next32()<half ); }

            //! random full integral type
            template <typename T> inline
            T full() throw()
            {
                T ans(0);
                
                static const size_t tbits = sizeof(T)*8;
                const  size_t       nbits = bits;
                size_t              rbits = 0;
                do
                {
                    (ans <<= nbits) |= T(next32());
                    rbits += nbits;
                }
                while(rbits<tbits);
                return ans;
            }

            //! random unsigned integral in 0..X-1
            template <typename T> inline
            T __lt(const T X) throw()
            {
                return ( (X<=0) ? T(0) : ( full<T>() % X ) );
            }

            //! 0..n-1
            inline size_t lt(const size_t n) throw()
            {
                return __lt<size_t>(n);
            }

            //! returns in 0..n
            inline size_t leq( size_t n ) throw()
            {
                return full<size_t>() % (++n);
            }

            //! Knuth shuffle of a[0..n-1]
            template <typename T>
            inline void shuffle( T *a, const size_t n ) throw()
            {
                assert(!(NULL==a&&n>0));
                if( n > 1 )
                {
                    for( size_t i=n-1;i>0;--i)
                    {
                        const size_t j   = leq(i);
                        bswap( a[i], a[j] );
                    }
                }
            }

            //! Knuth co shuffle of a[0..n-1] and b[0..n-1]
            template <typename T, typename U>
            inline void shuffle( T *a, U *b, const size_t n) throw()
            {
                assert(!(NULL==a&&n>0));
                if( n > 1 )
                {
                    for( size_t i=n-1;i>0;--i)
                    {
                        const size_t j   = leq(i);
                        bswap( a[i], a[j] );
                        bswap( b[i], b[j] );
                    }
                }
            }

            //! converson in float|double|int|string|point2d in unit square|point3d in unit box
            template <typename T> T get();
            

            static Bits & Simple(); //!< isaac4
            static Bits & Crypto(); //!< isaac8



            //! on unit circle
            template <typename T>
            inline void onCircle(T &x, T&y) throw()
            {
            PROBE:
                {
                    const T x1 = symm<T>();
                    const T x2 = symm<T>();
                    const T x1_sq = x1*x1;
                    const T x2_sq = x2*x2;
                    const T den   = x1_sq+x2_sq;
                    if(den>=T(1)) goto PROBE;
                    x = (x1_sq-x2_sq)/den;
                    y = (x1*x2)/den;
                    y+=y;
                }
            }

            //! in unit disk
            template <typename T>
            inline void inDisk(T &x, T&y) throw()
            {
            PROBE:
                {
                    const T x1 = symm<T>();
                    const T x2 = symm<T>();
                    const T x1_sq = x1*x1;
                    const T x2_sq = x2*x2;
                    const T den   = x1_sq+x2_sq;
                    if(den>=T(1)) goto PROBE;
                    x = x1;
                    y = x2;
                }
            }

            //! wrapper for point2d
            template <typename POINT>
            inline POINT onCircle(void) throw()
            {
                POINT p;
                onCircle<typename POINT::type>(p.x,p.y);
                return p;
            }

            //! wrapper for point3d
            template <typename POINT>
            inline POINT inDisk(void) throw()
            {
                POINT p;
                inDisk<typename POINT::type>(p.x,p.y);
                return p;
            }

            static inline float  __sqrt(const float  x) throw() { return sqrtf(x); }
            static inline double __sqrt(const double x) throw() { return sqrt(x);  }

            //! on unit circle
            template <typename T>
            inline void onSphere(T &x, T&y, T &z) throw()
            {
                static const T __one(1);
            PROBE:
                {
                    const T x1  = symm<T>();
                    const T x2  = symm<T>();
                    const T x12 = x1*x1;
                    const T x22 = x2*x2;
                    const T sum = x12+x22;
                    if(sum>=__one) goto PROBE;
                    const T oms = __one-sum;
                    const T sqs = __sqrt(oms);
                    const T hx  = x1 * sqs;
                    const T hy  = x2 * sqs;
                    z = __one-(sum+sum);
                    x = hx+hx;
                    y = hy+hy;
                }
            }

            //! wrapper for point3d
            template <typename POINT>
            inline POINT onSphere(void) throw()
            {
                POINT p;
                onSphere<typename POINT::type>(p.x,p.y,p.z);
                return p;
            }

            //! within the unit ball
            template <typename T>
            inline void inBall(T &x, T&y, T &z) throw()
            {
                static const T __one(1);
            PROBE:
                {
                    const T xx  = symm<T>();
                    const T yy  = symm<T>();
                    const T zz  = symm<T>();
                    const T sq  = xx*xx+yy*yy+zz*zz;
                    if(sq>=__one) goto PROBE;
                    x = xx;
                    y = yy;
                    z = zz;
                }
            }

            template <typename POINT>
            inline POINT inBall(void) throw()
            {
                POINT p;
                inBall<typename POINT::type>(p.x,p.y,p.z);
                return p;
            }

        protected:
            explicit Bits(const uint32_t maxValue) throw();
            
        private:
            YOCTO_DISABLE_ASSIGN(Bits);
        };

        template <> inline float Bits::to<float>() throw()
        {
            return (0.5f+float(next32()))/denF;
        }

        template <> inline double Bits::to<double>() throw()
        {
            return (0.5+double(next32()))/denD;
        }

        template <> inline float Bits::symm<float>() throw()
        {
            return ( float(next32()) + addNumF ) / halfDenF;
        }

        template <> inline double Bits::symm<double>() throw()
        {
            return ( double(next32()) + addNumD ) / halfDenD;
        }

        //______________________________________________________________________
        //
        //! sample base and rand()
        //______________________________________________________________________
        class cstdbits : public Bits
        {
        public:
            explicit cstdbits() throw();
            virtual ~cstdbits() throw();
            virtual uint32_t next32() throw();
            virtual void     reseed(Bits &) throw();
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(cstdbits);
        };
        

    }
}

#endif

