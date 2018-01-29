#ifndef YOCTO_INK_PIXMAPS_INCLUDED
#define YOCTO_INK_PIXMAPS_INCLUDED 1

#include "yocto/ink/color/conv.hpp"
#include "yocto/ink/pixmap.hpp"
#include "yocto/ink/color/yuv.hpp"
#include "yocto/sequence/vector.hpp"

namespace yocto
{
    namespace Ink
    {
        typedef Pixmap<float>   PixmapF;
        typedef Pixmap<uint8_t> PixmapU;
        typedef Pixmap<RGB>     PixmapRGB;
        typedef Pixmap<RGBA>    PixmapRGBA;
        typedef Pixmap<YUV>     PixmapYUV;

        template <typename T>
        class Pixmaps : public vector< typename Pixmap<T>::Pointer >
        {
        public:
            typedef typename Pixmap<T>::Pointer PointerType;
            typedef vector< PointerType >       ArrayType;

            //! default ctor
            explicit Pixmaps() throw() : ArrayType() {}

            //! default dtor
            virtual ~Pixmaps() throw() {}

            //! ctor to reserve memory
            explicit Pixmaps(const size_t n) : ArrayType(n,as_capacity) {}

            explicit Pixmaps(const size_t n, const unit_t w, const unit_t h) :
            ArrayType(n,as_capacity)
            {
                for(size_t i=0;i<n;++i)
                {
                    append(w,h);
                }
            }

            inline void append( Pixmap<T> *pxm )
            {
                const PointerType p(pxm);
                this->push_back(p);
            }

            inline void append(const unit_t w, const unit_t h)
            {
                append( new Pixmap<T>(w,h) );
            }

            inline void ldz() throw()
            {
                ArrayType &self = *this;
                for(size_t i=self.size();i>0;--i) self[i]->ldz();
            }
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Pixmaps);
        };

    }
}
#endif

