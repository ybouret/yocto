#ifndef YOCTO_INK_DRAW_PUTPIXEL_INCLUDED
#define YOCTO_INK_DRAW_PUTPIXEL_INCLUDED 1

#include "yocto/ink/color/pixel.hpp"
#include "yocto/container/tuple.hpp"
#include "yocto/ink/pixmap.hpp"

namespace yocto
{
    namespace Ink
    {
        namespace Draw
        {
            
#define YOCTO_INK_DRAW_ARGS void  (*proc)( Pixmap<T> &bg, const coord &q, void *args), void   *args

            struct PutPixel
            {


                template <typename T> static inline
                void Copy( Pixmap<T> &pxm, const coord &q, void *args) throw()
                {
                    assert(args); pxm[q] = *static_cast<T*>(args);
                }

                template <typename T>
                YOCTO_PAIR_DECL(TEMPLATE,BlendInfo,const T,C,const uint8_t,alpha);
               /* inline BlendInfo(const T &userC, const uint8_t &userAlpha) :
                C(userC), alpha(userAlpha) {}*/
                YOCTO_PAIR_END();
                
                template <typename T> static inline
                void Blend( Pixmap<T> &pxm, const coord &q, void *args) throw()
                {
                    assert(args);
                    const BlendInfo<T> &blend = *static_cast<BlendInfo<T> *>(args);
                    T &bg = pxm[q];
                    bg  = Pixel<T>::Blend(blend.C, bg, blend.alpha);
                }
            };
        }
    }
}

#endif

