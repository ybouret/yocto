
#ifndef YOCTO_IPSO_MESH_CURVILINEAR_INCLUDED
#define YOCTO_IPSO_MESH_CURVILINEAR_INCLUDED 1

#include "yocto/ipso/mesh/rectilinear.hpp"

namespace yocto
{
    namespace ipso
    {
        //! a curvilinear mesh
        template <typename T,const size_t DIMENSION>
        class  curvilinear_mesh :
        public mesh_info
        {
        public:
            static const size_t DIM = DIMENSION; //! over patch1D::DIM
            YOCTO_ARGUMENTS_DECL_T;
            typedef typename coord_for<DIMENSION>::type    coord_type;
            typedef subset<coord_type>                     subset_type;
            typedef typename field_for<T,DIMENSION>::type  axis_type;
            typedef typename patch_for<DIMENSION>::type    patch_type;
            typedef box<T,DIMENSION>                       box_type;

            //! default ctor
            inline virtual ~curvilinear_mesh() throw() {}

            //! setup the mesh based on a subset
            /**
             \param names the different axis names
             \param sub the subset, which can be used to transert data
             */
            inline explicit curvilinear_mesh(const array<string> &names,
                                             const subset_type   &sub ) :
            mesh_info(DIMENSION), axis_handle()
            {
                setup(names,sub);
            }
            
            inline axis_type       &X() throw()       { assert(dimension>=1); return *axis_handle[0]; }
            inline const axis_type &X() const throw() { assert(dimension>=1); return *axis_handle[0]; }
            
            inline axis_type       &Y() throw()       { assert(dimension>=2); return *axis_handle[1]; }
            inline const axis_type &Y() const throw() { assert(dimension>=2); return *axis_handle[1]; }
            
            inline axis_type       &Z() throw()       { assert(dimension>=3); return *axis_handle[2]; }
            inline const axis_type &Z() const throw() { assert(dimension>=3); return *axis_handle[2]; }

            //! map to a regular grid
            inline void map_regular(const box_type  &b,
                                    const patch_type inner)
            {
                __map(b,inner,int2type<DIMENSION>() );
            }

            //! save structure after  VTK::Header
            inline void vtk( ios::ostream &fp ) const
            {
                const patch_type &p = *axis_handle[0];
                fp << "DATASET STRUCTURED_GRID\n";
                fp << "DIMENSIONS";
                for(size_t dim=0;dim<DIM;++dim)
                {
                    fp(" %u", unsigned(__coord(p.width,dim)));
                }
                for(size_t dim=DIM;dim<3;++dim)
                {
                    fp  << " 1";
                }
                fp << '\n';
                fp("POINTS %u ",unsigned(p.items)); VTK::OutputScalarType<T>(fp); fp << '\n';
                output_vtk(fp,int2type<DIMENSION>());
            }
            
            
        protected:
            axis_type *axis_handle[DIMENSION];
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(curvilinear_mesh);
            
            void setup(const array<string> &names,
                       const subset_type   &sub)
            {
                assert(names.size()==DIMENSION);
                memset(axis_handle,0,sizeof(axis_handle));
                
                for(size_t dim=0;dim<DIMENSION;++dim)
                {
                    axis_type       &axis = axis_db.build<axis_type>(names[dim+1],sub.outer);
                    axis_handle[dim] = &axis;
                    axis_info.append(axis);
                }
            }
            
            ////////////////////////////////////////////////////////////////////
            inline void __map(const box_type   &b,
                              const patch_type &inner,
                              int2type<1> )
            {
                const patch_type &p    = *axis_handle[0];
                const_type        xmin  = b.lower;
                const_type        xlen  = b.width;
                const coord1D     xden  = inner.upper-inner.lower;
                if(xden<=0) throw exception("curvilear_mesh.map_regular(bad inner %s)", __coord_name(0) );

                axis_type & _X = X();
                for(coord1D i=p.lower;i<=p.upper;++i)
                {
                    _X[i] = xmin + (xlen*(i-inner.lower))/xden;
                }
            }
            
            inline void __map(const box_type   &b,
                              const patch_type &inner,
                              int2type<2> )
            {
                const patch_type &p     = *axis_handle[0];
                const_type        xmin  = b.lower.x;
                const_type        xlen  = b.width.x;
                const coord1D     xden  = inner.upper.x-inner.lower.x;
                
                const_type        ymin  = b.lower.y;
                const_type        ylen  = b.width.y;
                const coord1D     yden  = inner.upper.y-inner.lower.y;
                
                if(xden<=0) throw exception("curvilear_mesh.map_regular(bad inner %s)", __coord_name(0) );
                if(yden<=0) throw exception("curvilear_mesh.map_regular(bad inner %s)", __coord_name(1) );

                axis_type & _X = X();
                axis_type & _Y = Y();
                for(coord1D j=p.lower.y;j<=p.upper.y;++j)
                {
                    const_type yval = ymin + ( ylen * (j-inner.lower.y))/yden;
                    for(coord1D i=p.lower.x;i<=p.upper.x;++i)
                    {
                        const_type xval = xmin + (xlen*(i-inner.lower.x))/xden;
                        _X[j][i] = xval;
                        _Y[j][i] = yval;
                    }
                }
            }
            
            inline void __map(const box_type   &b,
                              const patch_type &inner,
                              int2type<3> )
            {
                const patch_type &p     = *axis_handle[0];
                const_type        xmin  = b.lower.x;
                const_type        xlen  = b.width.x;
                const coord1D     xden  = inner.upper.x-inner.lower.x;
                
                const_type        ymin  = b.lower.y;
                const_type        ylen  = b.width.y;
                const coord1D     yden  = inner.upper.y-inner.lower.y;
                
                const_type        zmin  = b.lower.z;
                const_type        zlen  = b.width.z;
                const coord1D     zden  = inner.upper.z-inner.lower.z;
                
                if(xden<=0) throw exception("curvilear_mesh.map_regular(bad inner %s)", __coord_name(0) );
                if(yden<=0) throw exception("curvilear_mesh.map_regular(bad inner %s)", __coord_name(1) );
                if(zden<=0) throw exception("curvilear_mesh.map_regular(bad inner %s)", __coord_name(2) );

                axis_type & _X = X();
                axis_type & _Y = Y();
                axis_type & _Z = Z();

                for(coord1D k=p.lower.z;k<=p.upper.z;++k)
                {
                    const_type zval = zmin + ( zlen * (k-inner.lower.z))/zden;
                    for(coord1D j=p.lower.y;j<=p.upper.y;++j)
                    {
                        const_type yval = ymin + ( ylen * (j-inner.lower.y))/yden;
                        for(coord1D i=p.lower.x;i<=p.upper.x;++i)
                        {
                            const_type xval = xmin + (xlen*(i-inner.lower.x))/xden;
                            _X[k][j][i] = xval;
                            _Y[k][j][i] = yval;
                            _Z[k][j][i] = zval;
                        }
                    }
                }
            }
            
            
            
            ////////////////////////////////////////////////////////////////////
            inline void output_vtk( ios::ostream &fp, int2type<1> ) const
            {
                const patch_type &p = *axis_handle[0];
                for(coord1D i=p.lower;i<=p.upper;++i)
                {
                    fp("%.15g 0 0\n", X()[i]);
                }
            }
            
            inline void output_vtk( ios::ostream &fp, int2type<2> ) const
            {
                const patch_type &p = *axis_handle[0];
                for(coord1D j=p.lower.y;j<=p.upper.y;++j)
                {
                    for(coord1D i=p.lower.x;i<=p.upper.x;++i)
                    {
                        fp("%.15g %.15g 0\n",X()[j][i],Y()[j][i]);
                    }
                }
            }
            
            inline void output_vtk( ios::ostream &fp, int2type<3> ) const
            {
                const patch_type &p = *axis_handle[0];
                for(coord1D k=p.lower.z;k<=p.upper.z;++k)
                {
                    for(coord1D j=p.lower.y;j<=p.upper.y;++j)
                    {
                        for(coord1D i=p.lower.x;i<=p.upper.x;++i)
                        {
                            fp("%.15g %.15g %.15g\n",X()[k][j][i],Y()[k][j][i],Z()[k][j][i]);
                        }
                    }
                }
            }
            
        };
        
    }
}

#endif

