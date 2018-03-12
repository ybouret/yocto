

#ifndef YOCTO_INK_OPS_SCHARR_INCLUDED
#define YOCTO_INK_OPS_SCHARR_INCLUDED 1

#include "yocto/ink/ops/stencil.hpp"

namespace yocto
{
    namespace Ink
    {
        class Scharr3Y : public Stencil
        {
        public:
            inline explicit Scharr3Y() : Stencil("Scharr3Y",1,1)
            {
                static const float mtx[9] = { -3, -10, -3, 0, 0, 0, 3, 10, 3 };
                memcpy(entry,mtx,sizeof(mtx));
                compile();
            }

            inline virtual ~Scharr3Y() throw()
            {
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Scharr3Y);
        };

        class Scharr3X : public Stencil
        {
        public:
            inline explicit Scharr3X() : Stencil("Scharr3X",1,1)
            {
                static const float mtx[9] = { -3, 0, 3, -10, 0, 10, -3, 0, 3 };
                memcpy(entry,mtx,sizeof(mtx));
                compile();
            }

            inline virtual ~Scharr3X() throw()
            {
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Scharr3X);
        };


        class Scharr5Y : public Stencil
        {
        public:
            inline explicit Scharr5Y() : Stencil("Scharr5Y",2,2)
            {
                static const float mtx[25] =
                {
                    -1,-2,-3,-2,-1,
                    -1,-2,-6,-2,-1,
                    0,0,0,0,0,
                    1,2,6,2,1,
                    1,2,3,2,1
                };
                memcpy(entry,mtx,sizeof(mtx));
                compile();
            }

            inline virtual ~Scharr5Y() throw()
            {
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Scharr5Y);
        };

        class Scharr5X : public Stencil
        {
        public:
            inline explicit Scharr5X() : Stencil("Scharr5X",2,2)
            {
                static const float mtx[25] =
                {
                    -1,-1,0,1,1,
                    -2,-2,0,2,2,
                    -3,-6,0,6,3,
                    -2,-2,0,2,2,
                    -1,-1,0,1,1
                };
                memcpy(entry,mtx,sizeof(mtx));
                compile();
            }

            inline virtual ~Scharr5X() throw()
            {
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Scharr5X);
        };

    }

}

#endif

