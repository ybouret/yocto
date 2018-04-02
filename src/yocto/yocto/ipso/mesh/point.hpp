
#ifndef YOCTO_IPSO_MESH_POINT_INCLUDED
#define YOCTO_IPSO_MESH_POINT_INCLUDED 1

#include "yocto/ipso/mesh.hpp"
#include "yocto/ipso/vertex.hpp"
#include "yocto/ipso/format/vtk.hpp"

namespace yocto
{
    namespace ipso
    {
        template <typename T,const size_t DIMENSION>
        class point_mesh :
        public mesh_info
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;

            static const size_t DIM = DIMENSION; //! over patch1D::DIM
            typedef typename vertex_for<T,DIMENSION>::type vertex_type;
            typedef field1D<T>                             axis_type;
            inline virtual ~point_mesh() throw() {}

            //! a point mesh based on a 1D subset
            inline explicit point_mesh(const string          &id,
                                       const array<string>   &names,
                                       const subset<coord1D> &sub ) :
            mesh_info(id,DIMENSION),  axis_handle()
            {
                setup(names,sub);
            }

            inline explicit point_mesh(const char            *id,
                                       const array<string>   &names,
                                       const subset<coord1D> &sub ) :
            mesh_info(id,DIMENSION),  axis_handle()
            {
                setup(names,sub);
            }

            inline axis_type       &X() throw()       { assert(dimension>=1); return *axis_handle[0]; }
            inline const axis_type &X() const throw() { assert(dimension>=1); return *axis_handle[0]; }

            inline axis_type       &Y() throw()       { assert(dimension>=2); return *axis_handle[1]; }
            inline const axis_type &Y() const throw() { assert(dimension>=2); return *axis_handle[1]; }

            inline axis_type       &Z() throw()       { assert(dimension>=3); return *axis_handle[2]; }
            inline const axis_type &Z() const throw() { assert(dimension>=3); return *axis_handle[2]; }
            
            // reconstruct a vertex from coordinates
            inline vertex_type operator[](const coord1D i) const throw()
            {
                assert(axis_handle[0]->contains(i));
                vertex_type ans;
                T          *p = (T*)&ans;
                for(size_t dim=0;dim<DIMENSION;++dim)
                {
                     p[dim] = (*axis_handle[dim])[i];
                }
                return ans;
            }

            //! map a regular point mesh from ini to end
            inline void map_regular(const vertex_type ini,
                                    const vertex_type end,
                                    const patch1D     inner)
            {
                const vertex_type del = end-ini;
                const coord1D     den = inner.upper-inner.lower;
                if(den<=0) throw exception("point_mesh.map_regular(bad inner patch)");
                const patch1D    &p   = *axis_handle[0];
                const T *w = (const T *)&del;
                const T *q = (const T *)&ini;
                for(coord1D i=p.lower;i<=p.upper;++i)
                {
                    const coord1D num = i - inner.lower;
                    for(size_t dim=0;dim<DIMENSION;++dim)
                    {
                        const T v = q[dim] + (num*w[dim])/den;
                        (*axis_handle[dim])[i] = v;
                    }
                }
            }
            
            //! 2D and 3D map on circle
            inline void map_circle(const patch1D full, const_type radius=1)
            {
                assert(DIMENSION>=2);
                static const T    amax   = math::numeric<T>::two_pi;
                const coord1D     den    = full.width;
                const T           dtheta = amax/den;
                const patch1D    &p      = *axis_handle[0];
                for(size_t dim=0;dim<DIMENSION;++dim)
                {
                    axis_handle[dim]->ldz();
                }
                for(coord1D i=p.lower;i<=p.upper;++i)
                {
                    const T theta = (i*dtheta);
                    const T c     = math::Cos(theta);
                    const T s     = math::Sin(theta);
                    X()[i] = radius*c;
                    Y()[i] = radius*s;
                }
            }
            
            // save to vtk after a VTK::Header
            void vtk( ios::ostream &fp, const bool periodic = false) const
            {
                const patch1D &p = *axis_handle[0];
                fp << "DATASET POLYDATA\n";
                fp << "POINTS "; fp("%u ",unsigned(p.items)); VTK::OutputScalarType<T>(fp); fp << '\n';
                for(coord1D i=p.lower;i<=p.upper;++i)
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
            void setup(const array<string>   &names,
                       const subset<coord1D> &sub )
            {
                assert(names.size()==DIM);
                memset(axis_handle,0,sizeof(axis_handle));
                for(size_t dim=0;dim<DIM;++dim)
                {
                    axis_type       &axis = axis_db.build<axis_type>(names[dim+1],sub.outer);
                    axis_handle[dim] = &axis;
                    axis_info.append(axis);
                }
            }
        };
    }
}

#endif


