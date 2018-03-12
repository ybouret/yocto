
#ifndef YOCTO_INK_OPS_SOBEL_INCLUDED
#define YOCTO_INK_OPS_SOBEL_INCLUDED 1

#include "yocto/ink/ops/stencil.hpp"

namespace yocto
{
    namespace Ink
    {
        class Sobel3Y : public Stencil
        {
        public:
            inline explicit Sobel3Y() : Stencil("Sobel3Y",1,1)
            {
                static const float mtx[9] = { -1, -2, -1, 0, 0, 0, 1, 2, 1 };
                memcpy(entry,mtx,sizeof(mtx));
                compile();
            }

            inline virtual ~Sobel3Y() throw()
            {
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Sobel3Y);
        };

        class Sobel3X : public Stencil
        {
        public:
            inline explicit Sobel3X() : Stencil("Sobel3X",1,1)
            {
                static const float mtx[9] = { -1, 0, 1, -2, 0, 2, -1, 0, 1 };
                memcpy(entry,mtx,sizeof(mtx));
                compile();
            }

            inline virtual ~Sobel3X() throw()
            {
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Sobel3X);
        };

        class Sobel5Y : public Stencil
        {
        public:
            inline explicit Sobel5Y() : Stencil("Sobel5Y",2,2)
            {
                static const float mtx[25] =
                {
                    -5, -8,-10, -8,-5,
                    -4,-10,-20,-10,-4,
                    0,  0,  0,  0,  0,
                    4, 10, 20, 10,  4,
                    5,  8, 10,  8,  5
                };
                memcpy(entry,mtx,sizeof(mtx));
                compile();
            }

            inline virtual ~Sobel5Y() throw()
            {
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Sobel5Y);
        };

        class Sobel5X : public Stencil
        {
        public:
            inline explicit Sobel5X() : Stencil("Sobel5X",2,2)
            {
                static const float mtx[25] =
                {
                    -5,-4,0,4,5,
                    -8,-10,0,10,8,
                    -10,-20,0,20,10,
                    -8,-10,0,10,8,
                    -5,-4,0,4,5,
                };
                memcpy(entry,mtx,sizeof(mtx));
                compile();
            }

            inline virtual ~Sobel5X() throw()
            {
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Sobel5X);
        };

    }
}

#endif

