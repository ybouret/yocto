#include "yocto/ipso/vtk.hpp"

namespace yocto
{
    namespace ipso
    {
        void VTK:: Header(ios::ostream &fp,
                          const string &title)
        {
            fp << "# vtk DataFile Version 3.0\n";
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
        
        void VTK:: StructuredPoints(ios::ostream &fp,
                                    const size_t  DIM,
                                    const unit_t *width,
                                    const unit_t *lower,
                                    const size_t items)
        {
            assert(DIM>0);
            assert(DIM<=3);
            assert(width);
            assert(lower);
            assert(items>0);
            
            fp << "DATASET STRUCTURED_POINTS\n";
            
            fp << "DIMENSIONS";
            for(size_t i=0;i<DIM;++i)
            {
                fp(" %u", unsigned(width[i]) );
            }
            for(size_t i=DIM;i<3;++i)
            {
                fp(" 1");
            }
            fp << "\n";
            
            fp << "ORIGIN";
            for(size_t i=0;i<DIM;++i)
            {
                fp(" %d", int(lower[i]) );
            }
            for(size_t i=DIM;i<3;++i)
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
            fp << "POINT_DATA "; fp(" %u\n", unsigned(items) );
        }
        
        // float
        template <>
        void VTK:: OutputType<float>(ios::ostream &fp)
        {
            fp << "float 1";
        }
        
        template <>
        void VTK:: OutputType<double>(ios::ostream &fp)
        {
            fp << "double 1";
        }
        
        // double
        template <>
        void VTK:: OutputValue<float>(ios::ostream &fp,const float &f)
        {
            fp("%g",f);
        }

        template <>
        void VTK:: OutputValue<double>(ios::ostream &fp,const double &f)
        {
            fp("%g",f);
        }
        
        // point2d<float>
        template <>
        void VTK:: OutputType< point2d<float> >(ios::ostream &fp)
        {
            fp << "float 2";
        }
        
        template <>
        void VTK:: OutputValue< point2d<float> >(ios::ostream &fp,const point2d<float> &f)
        {
            fp("%g %g",f.x,f.y);
        }
        
        // point2d<double>
        template <>
        void VTK:: OutputType< point2d<double> >(ios::ostream &fp)
        {
            fp << "double 2";
        }
        
        template <>
        void VTK:: OutputValue< point2d<double> >(ios::ostream &fp,const point2d<double> &f)
        {
            fp("%g %g",f.x,f.y);
        }
        
        // point3d<float>
        template <>
        void VTK:: OutputType< point3d<float> >(ios::ostream &fp)
        {
            fp << "float 3";
        }
        
        template <>
        void VTK:: OutputValue< point3d<float> >(ios::ostream &fp,const point3d<float> &f)
        {
            fp("%g %g %g",f.x,f.y,f.z);
        }
        
        // point3d<double>
        template <>
        void VTK:: OutputType< point3d<double> >(ios::ostream &fp)
        {
            fp << "float 3";
        }
        
        template <>
        void VTK:: OutputValue< point3d<double> >(ios::ostream &fp,const point3d<double> &f)
        {
            fp("%g %g %g",f.x,f.y,f.z);
        }
        
        
    }
}

