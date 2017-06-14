#ifndef YOCTO_FAME_FORMAT_VTK_INCLUDED
#define YOCTO_FAME_FORMAT_VTK_INCLUDED 1


#include "yocto/fame/field.hpp"
#include "yocto/ios/ostream.hpp"

namespace yocto
{
    namespace fame
    {


        struct VTK
        {
            static inline void Header(ios::ostream &fp,
                                      const string &title)
            {
                fp << "# vtk DataFile Version 2.0\n";
                fp << "# ";
                const size_t n = min_of<size_t>(title.size(),250);
                for(size_t i=0;i<n;++i)
                {
                    char C = title[i];
                    if(C<32) C='_';
                    fp.write(C);
                }
                fp << "\n";
                fp << "ASCII\n";
            }

            template <typename COORD>
            static inline void StructuredPoints(ios::ostream          &fp,
                                                const layout<COORD>   &L)
            {
                fp << "DATASET STRUCTURED_POINTS\n";

                fp << "DIMENSIONS";
                for(size_t i=0;i<YOCTO_FAME_DIM_OF(COORD);++i)
                {
                    fp(" %u", unsigned(__coord(L.width,i)) );
                }
                for(size_t i=YOCTO_FAME_DIM_OF(COORD);i<3;++i)
                {
                    fp(" 1");
                }
                fp << "\n";

                fp << "ORIGIN";
                for(size_t i=0;i<YOCTO_FAME_DIM_OF(COORD);++i)
                {
                    fp(" %d", int(__coord(L.lower,i)) );
                }
                for(size_t i=YOCTO_FAME_DIM_OF(COORD);i<3;++i)
                {
                    fp(" 0");
                }
                fp << "\n";


                fp << "SPACING";
                for(size_t i=0;i<3;++i)
                {
                    fp << " 1";
                }
                fp << "\n";
            }

            template <typename T>
            static void OutputType(ios::ostream &fp);

            template <typename T>
            static void OutputValue(ios::ostream &fp, const T &);

            template <typename T,typename COORD> struct OutputScalars;

            template <typename T>
            struct OutputScalars<T,coord1d>
            {
                static inline
                void Write(ios::ostream          &fp,
                           const field<T,coord1d> &F,
                           const layout<coord1d>  &L)
                {
                    for(coord1d x=L.lower;x<=L.upper;++x)
                    {
                        OutputValue(fp,F.at(x)); fp << "\n";
                    }
                }
            };

            template <typename T>
            struct OutputScalars<T,coord2d>
            {
                static inline
                void Write(ios::ostream          &fp,
                           const field<T,coord2d> &F,
                           const layout<coord2d>  &L)
                {
                    for(coord1d y=L.lower.y;y<=L.upper.y;++y)
                    {
                        for(coord1d x=L.lower.x;x<=L.upper.x;++x)
                        {
                            OutputValue(fp,F.at(coord2d(x,y))); fp << "\n";
                        }
                    }
                }
            };

            template <typename T>
            struct OutputScalars<T,coord3d>
            {
                static inline
                void Write(ios::ostream          &fp,
                           const field<T,coord3d> &F,
                           const layout<coord3d>  &L)
                {
                    for(coord1d z=L.lower.z;z<=L.upper.z;++z)
                    {
                        for(coord1d y=L.lower.y;y<=L.upper.y;++y)
                        {
                            for(coord1d x=L.lower.x;x<=L.upper.x;++x)
                            {
                                OutputValue(fp,F.at(coord3d(x,y,z))); fp << "\n";
                            }
                        }
                    }
                }
            };




            template <typename T,typename COORD> static inline
            void SaveScalars(ios::ostream         &fp,
                             const string         &label,
                             const field<T,COORD> &F,
                             const layout<COORD>  &L)
            {
                fp << "POINT_DATA "; fp(" %u\n", unsigned(L.items) );
                fp << "SCALARS " << label << ' ';
                OutputType<T>(fp);
                fp << "\n";
                fp << "LOOKUP_TABLE " << label << "\n";
                OutputScalars<T,COORD>::Write(fp,F,L);
            }


            template <typename COORD>
            static inline void SaveLayout(ios::ostream  &fp,
                                          const string  &title,
                                          const layout<COORD> &L)
            {
                Header(fp,title);
                StructuredPoints<COORD>(fp,L);
            }


        };



        template <>
        inline void VTK:: OutputType<float>(ios::ostream &fp)
        {
            fp << "float 1";
        }

        template <>
        inline void VTK:: OutputValue<float>(ios::ostream &fp,const float &f)
        {
            fp("%g",f);
        }


        template <>
        inline void VTK:: OutputType<double>(ios::ostream &fp)
        {
            fp << "double 1";
        }
        
        template <>
        inline void VTK:: OutputValue<double>(ios::ostream &fp,const double &f)
        {
            fp("%g",f);
        }
        
        
        
    }
}

#endif
