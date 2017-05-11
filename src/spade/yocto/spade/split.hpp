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

                explicit In1D(const Layout1D &full,
                              const size_t    ncpu);
                virtual ~In1D() throw();

                Layout1D operator()(const coord1D rank) const;

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(In1D);
            };


            class In2D : public Layout2D
            {
            public:
                const coord2D sizes;
                const size_t  size;

                explicit In2D(const Layout2D &full,
                              const size_t    xcpu,
                              const size_t    ycpu);
                virtual ~In2D() throw();

                Layout2D operator()(const coord1D rank) const;

                // TODO: implement rank to x,y and reverse

                //! get ranks for rank<size
                coord2D getRanks(const coord1D rank) const throw();

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(In2D);
            };


            class In3D : public Layout3D
            {
            public:
                const coord3D sizes;
                const coord1D zcof;
                const size_t  size;

                explicit In3D(const Layout3D &full,
                              const size_t    xcpu,
                              const size_t    ycpu,
                              const size_t    zcpu);
                virtual ~In3D() throw();

                //! get ranks for rank<size
                coord3D getRanks(const coord1D rank) const throw();

                Layout3D operator()(const coord1D rank) const throw();


            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(In3D);
            };

            static coord2D DispatchCPUs(const Layout2D &L, const size_t cpus);

        };
    }
}


#endif
