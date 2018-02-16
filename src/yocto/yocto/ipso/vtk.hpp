#ifndef YOCTO_IPSO_VTK_INCLUDED
#define YOCTO_IPSO_VTK_INCLUDED 1

#include "yocto/ipso/field3d.hpp"
#include "yocto/ios/ostream.hpp"

namespace yocto
{
    namespace ipso
    {
        struct VTK
        {
            static void Header(ios::ostream &fp,
                               const string &title);
            
            static void StructuredPoints(ios::ostream &fp,
                                         const size_t  DIM,
                                         const unit_t *width,
                                         const unit_t *lower,
                                         const size_t  items);
            
            template <typename COORD>
            static inline void StructuredPoints(ios::ostream       &fp,
                                                const patch<COORD> &p)
            {
                StructuredPoints(fp,
                                 YOCTO_IPSO_DIM_OF(COORD),
                                 &__coord(p.width,0),
                                 &__coord(p.lower,0),
                                 p.items);
            }
            
            template <typename T> static void OutputType(ios::ostream &fp);
            template <typename T> static void OutputValue(ios::ostream &fp, const T &);
            
            template <typename T,typename COORD> struct OutputScalars;
            
            
        };
    }
}

#endif

