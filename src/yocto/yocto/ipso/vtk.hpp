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
            
            template <typename T,template <typename> class FIELD> struct OutputScalars;
            
            template <typename T>
            struct OutputScalars<T,field1D>
            {
                static inline void Write(ios::ostream     &fp,
                                         const field1D<T> &f,
                                         const patch1D    &p)
                {
                    for(coord1D x=p.lower;x<=p.upper;++x)
                    {
                        OutputValue(fp,f[x]); fp << "\n";
                    }
                }
            };
            
            template <typename T>
            struct OutputScalars<T,field2D>
            {
                static inline void Write(ios::ostream     &fp,
                                         const field2D<T> &f,
                                         const patch2D    &p)
                {
                    for(coord1D y=p.lower.y;y<=p.upper.y;++y)
                    {
                        for(coord1D x=p.lower.x;x<=p.upper.x;++x)
                        {
                            OutputValue(fp,f[y][x]); fp << "\n";
                        }
                    }
                }
            };
            
            template <typename T>
            struct OutputScalars<T,field3D>
            {
                static inline void Write(ios::ostream     &fp,
                                         const field3D<T> &f,
                                         const patch3D    &p)
                {
                    for(coord1D z=p.lower.z;z<=p.upper.z;++z)
                    {
                        for(coord1D y=p.lower.y;y<=p.upper.y;++y)
                        {
                            for(coord1D x=p.lower.x;x<=p.upper.x;++x)
                            {
                                OutputValue(fp,f[z][y][x]); fp << "\n";
                            }
                        }
                    }
                }
            };
            
            template <typename T,template <typename> class FIELD> static inline
            void SaveScalars(ios::ostream                        &fp,
                             const FIELD<T>                      &f,
                             const typename FIELD<T>::patch_type &p)
            {
                fp << "SCALARS " << f.name << ' ';
                OutputType<T>(fp);
                fp << "\n";
                fp << "LOOKUP_TABLE " << f.name << "\n";
                OutputScalars<T,FIELD>::Write(fp,f,p);
            }
            
            template <typename T,template <typename> class FIELD> static inline
            void InitSaveScalars(ios::ostream                        &fp,
                                 const string                        &title,
                                 const FIELD<T>                      &f,
                                 const typename FIELD<T>::patch_type &p )
            {
                Header(fp,title);
                StructuredPoints(fp,p);
                SaveScalars(fp,f,p);
            }
            
            
            
        };
    }
}

#endif

