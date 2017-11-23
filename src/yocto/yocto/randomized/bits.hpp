#ifndef YOCTO_RANDOMIZED_BITS_INCLUDED
#define YOCTO_RANDOMIZED_BITS_INCLUDED 1

#include "yocto/memory/buffer.hpp"

namespace yocto
{
    namespace Randomized
    {

        //! 32 bits interface
        class Bits : public object
        {
        public:
            const uint32_t span;     //!< returns in 0..span
            const double   denD;     //!< double(span)+1.0
            const float    denF;     //!< float(span)+1.0f
            const double   halfDenD; //!< denD/2
            const float    halfDenF; //!< denF/2
            const double   addNumD;  //!< 0.5-halfDenD
            const float    addNumF;  //!< 0.5f-halfDenF
            virtual ~Bits() throw();

            virtual uint32_t next32() throw() = 0;

            inline float  nextFloat()  throw() { return (0.5f+float(next32()))/denF; }
            inline double nextDouble() throw() { return (0.5+double(next32()))/denD; }

            inline float  nextFloatSymm() throw()
            {
                return ( float(next32()) + addNumF ) / halfDenF;
            }

            inline double nextDoubleSymm() throw()
            {
                return ( double(next32()) + addNumD ) / halfDenD;
            }

        protected:
            explicit Bits(const uint32_t maxValue) throw();
            
        private:
            YOCTO_DISABLE_ASSIGN(Bits);
        };

        class cstdbits : public Bits
        {
        public:
            explicit cstdbits() throw();
            virtual ~cstdbits() throw();
            virtual uint32_t next32() throw();
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(cstdbits);
        };

    }
}

#endif

