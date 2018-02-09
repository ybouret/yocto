#ifndef YOCTO_INK_BLOB_INCLUDED
#define YOCTO_INK_BLOB_INCLUDED 1

#include "yocto/ink/pixmap.hpp"
#include "yocto/ink/color/named.hpp"
#include "yocto/ink/color/pixel.hpp"

namespace yocto
{
    namespace Ink
    {

        class Blob : public Pixmap<size_t>
        {
        public:
            explicit Blob( const Bitmap &bmp );
            virtual ~Blob() throw();

            template <typename T>
            inline void build( const Pixmap<T> &src )
            {
                YOCTO_INK_AREA_LIMITS(*this);

                for(unit_t j=ymax;j>=ymin;--j)
                {
                    Row                           &B_j = (*this)[j];
                    const typename Pixmap<T>::Row &S_j = src[j];
                    for(unit_t i=xmax;i>=xmin;--i)
                    {
                        
                        //! skip zero pixel
                        if( Pixel<T>::is_zero(S_j[i]) )
                        {
                            assert(0==B_j[i]);
                            continue;
                        }

                    }
                }

            }
            

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Blob);
        };

    }
}

#endif

