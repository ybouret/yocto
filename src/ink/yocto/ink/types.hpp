#ifndef YOCTO_INK_TYPES_INCLUDED
#define YOCTO_INK_TYPES_INCLUDED 1

#include "yocto/ipso/patch.hpp"
#include "yocto/math/types.hpp"

namespace yocto
{
    namespace Ink
    {
        typedef ipso::coord2D         coord;
        typedef ipso::patch2D         patch;
        typedef float                 real_t;
        typedef math::complex<real_t> cplx_t;

#define YOCTO_INK_R2GS 0.2126f
#define YOCTO_INK_G2GS 0.7152f
#define YOCTO_INK_B2GS 0.0722f
#define YOCTO_INK_F2B(x) (uint8_t(floorf((x)*255.0f+0.5f)))
        struct Core
        {
            static const  float   uFloat[256];   //!< 0.0f..1.0f
            static const  float   uFloatR[256]; //!< (0.0f..1.0f)*YOCTO_INK_R2GS
            static const  float   uFloatG[256]; //!< (0.0f..1.0f)*YOCTO_INK_G2GS
            static const  float   uFloatB[256]; //!< (0.0f..1.0f)*YOCTO_INK_B2GS

            static inline uint8_t Foat2Byte(const float x) throw() { return YOCTO_INK_F2B(x); }
            static inline float   GreyScaleF(const uint8_t R,const uint8_t G,const uint8_t B) throw()
            {
                return uFloatR[R] + uFloatG[R] + uFloatB[B];
            }

            static inline uint8_t GreyScale1(const uint8_t R,const uint8_t G,const uint8_t B) throw()
            {
                return YOCTO_INK_F2B(GreyScaleF(R,G,B));
            }
        };
    }
}

#endif

