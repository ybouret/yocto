#ifndef YOCTO_SPADE_SPLIT_INCLUDED
#define YOCTO_SPADE_SPLIT_INCLUDED 1

#include "yocto/spade/layout.hpp"

namespace yocto
{
    namespace spade
    {
        struct Split
        {
            static void Basic(const size_t size,
                              const size_t rank,
                              coord1D     &offset,
                              coord1D     &length);


            static Layout1D __in1D(const size_t    size,
                                   const size_t    rank,
                                   const Layout1D &full);

            class In1D : public Layout1D
            {
            public:
                const size_t size;

                explicit In1D(const Layout1D &full, const size_t ncpu);
                virtual ~In1D() throw();

                Layout1D operator()(const size_t rank) const;

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(In1D);
            };


            class In2D : public Layout2D
            {
            public:
                const size_t x_size;
                const size_t y_size;
                const size_t size;

                explicit In2D(const Layout2D &full, const size_t xcpu, const size_t ycpu);
                virtual ~In2D() throw();

                Layout2D operator()(const size_t rank) const;

                // TODO: implement rank to x,y and reverse
                
            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(In2D);
            };

        };
    }
}


#endif
