
#ifndef YOCTO_IPSO_MESH_POINT_INCLUDED
#define YOCTO_IPSO_MESH_POINT_INCLUDED 1

#include "yocto/ipso/mesh.hpp"
#include "yocto/ipso/format/vtk.hpp"

namespace yocto
{
    namespace ipso
    {
        template <typename T,const size_t _DIM>
        class point_mesh :
        public mesh_info,
        public patch1D
        {
        public:
            static const size_t DIM = _DIM; //! over patch1D::DIM
            typedef field1D<T> axis_type;
            inline virtual ~point_mesh() throw() {}

            inline explicit point_mesh( const array<string> &names, const patch1D full ) :
            mesh_info(DIM), patch1D(full), axis_handle()
            {
                std::cerr << "PointMesh<" << DIM << ">" << std::endl;
                setup(names);
            }

            inline axis_type       &X() throw()       { assert(dimension>=1); return *axis_handle[0]; }
            inline const axis_type &X() const throw() { assert(dimension>=1); return *axis_handle[0]; }

            inline axis_type       &Y() throw()       { assert(dimension>=2); return *axis_handle[1]; }
            inline const axis_type &Y() const throw() { assert(dimension>=2); return *axis_handle[1]; }

            inline axis_type       &Z() throw()       { assert(dimension>=3); return *axis_handle[2]; }
            inline const axis_type &Z() const throw() { assert(dimension>=3); return *axis_handle[2]; }

            void vtk( ios::ostream &fp, const bool periodic = false) const
            {
                fp << "DATASET POLYDATA\n";
                fp << "POINTS "; fp("%u ",unsigned(items)); VTK::OutputScalarType<T>(fp); fp << '\n';
                for(coord1D i=lower;i<=upper;++i)
                {
                    for(size_t dim=0;dim<DIM;++dim)
                    {
                        fp("%.15g",(*axis_handle[dim])[i]);
                        if(dim<2) fp << ' ';
                    }
                    for(size_t dim=DIM;dim<3;++dim)
                    {
                        fp << '0';
                        if(dim<2) fp << ' ';
                    }
                    fp << '\n';
                }
            }

        private:
            axis_type *axis_handle[DIM];
            YOCTO_DISABLE_COPY_AND_ASSIGN(point_mesh);
            void setup(const array<string> &names)
            {
                assert(names.size()==DIM);
                memset(axis_handle,0,sizeof(axis_handle));
                for(size_t dim=0;dim<DIM;++dim)
                {
                    axis_type       &axis = axis_db.build<axis_type>(names[dim+1],*this);
                    axis_handle[dim] = &axis;
                    axis_info.append(axis);
                }
            }
        };
    }
}

#endif


