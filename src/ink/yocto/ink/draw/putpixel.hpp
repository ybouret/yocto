#ifndef YOCTO_INK_DRAW_PUTPIXEL_INCLUDED
#define YOCTO_INK_DRAW_PUTPIXEL_INCLUDED 1

#include "yocto/ink/color/pixel.hpp"
#include "yocto/container/tuple.hpp"

namespace yocto
{
    namespace Ink
    {
        namespace Draw
        {
            struct PutPixel
            {


                template <typename T> static inline
                void Copy( T &bg, void *args) throw()
                {
                    assert(args); bg = *static_cast<T*>(args);
                }

                template <typename T>
                YOCTO_PAIR_DECL(TEMPLATE,BlendInfo,const T,C,const uint8_t,alpha);
               /* inline BlendInfo(const T &userC, const uint8_t &userAlpha) :
                C(userC), alpha(userAlpha) {}*/
                YOCTO_PAIR_END();
                
                template <typename T> static inline
                void Blend( T &bg, void *args) throw()
                {
                    assert(args);
                    const BlendInfo<T> &blend = *static_cast<BlendInfo<T> *>(args);
                    bg = Pixel<T>::Blend(blend.C, bg, blend.alpha);
                }
            };
        }
    }
}

#endif

