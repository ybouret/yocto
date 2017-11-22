#ifndef YOCTO_RANDOMIZED_BITS_INCLUDED
#define YOCTO_RANDOMIZED_BITS_INCLUDED 1

#include "yocto/memory/buffer.hpp"

namespace yocto
{
    namespace Randomized
    {
        //! interface to a underlying 32 bits random word generator
        class Bits : public object
        {
        public:
            virtual ~Bits() throw();
            
            //__________________________________________________________________
            //
            // virtual interface
            //__________________________________________________________________
            virtual  uint32_t next32() throw() = 0;
            inline   float    nextUnitFloat()  throw() { return ToUnitFloat(next32()); }
            inline   float    nextUnitDouble() throw() { return ToUnitDouble(next32()); }
            template <typename T>
            inline T nextBit() throw()
            {
                static const uint32_t half32 = 0x80000000;
                return (( next32() >= half32 ) ? T(1) : T(0));
            }
            
            template <typename T>
            inline T full() throw()
            {
                T ans(0);
                for(unsigned i=sizeof(T)*8;i>0;--i)
                {
                    (ans <<= 1) |= nextBit<T>();
                }
                return ans;
            }
            
            
            //__________________________________________________________________
            //
            // utility functions
            //__________________________________________________________________
            static inline float ToUnitFloat(const uint32_t u) throw()
            {
                return ( 0.5f + float(u) ) / 4294967296.0f;
            }
            
            static inline double ToUnitDouble(const uint32_t u) throw()
            {
                return (0.5+double(u)) / 4294967296.0;
            }
            
            static inline float ToSymmetricFloat(const uint32_t u) throw()
            {
                return (float(u)-2147483647.5f)/2147483648.0f;
            }
            
            static inline double ToSymmetricDouble(const uint32_t u) throw()
            {
                return (double(u)-2147483647.5)/2147483648.0;
            }
            
            //! 8 bits to 2 bits table
            static const  uint8_t Table2[256];
            
            //! compress a 32 bit word to 8 bits
            static inline uint8_t ToByte(const uint32_t u) throw()
            {
                return
                (Table2[(u>>24)&0xff] << 6) |
                (Table2[(u>>16)&0xff] << 4) |
                (Table2[(u>> 8)&0xff] << 2) |
                (Table2[u      &0xff]     );
                
            }
            
            void fill( memory::rw_buffer &buffer ) throw();
            
        protected:
            explicit Bits() throw();
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Bits);
        };
    }
}
#endif

