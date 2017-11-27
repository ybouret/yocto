#ifndef YOCTO_RANDOMIZED_BITS_INCLUDED
#define YOCTO_RANDOMIZED_BITS_INCLUDED 1

#include "yocto/memory/buffer.hpp"
#include "yocto/code/bswap.hpp"

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

            //! converson in float|double|int
            template <typename T>
            T get();
            

            static Bits & Simple();
            static Bits & Crypto();

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

