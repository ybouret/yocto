
#ifndef YOCTO_IPSO_MESH_CURVILINEAR_INCLUDED
#define YOCTO_IPSO_MESH_CURVILINEAR_INCLUDED 1

#include "yocto/ipso/mesh/rectilinear.hpp"

namespace yocto
{
    namespace ipso
    {
        template <typename T,const size_t _DIM>
        class  curvilinear_mesh :
        public mesh_info,
        public patch_for<_DIM>::type
        {
        public:
            static const size_t DIM = _DIM; //! over patch1D::DIM
            YOCTO_ARGUMENTS_DECL_T;
            typedef typename patch_for<_DIM>::type    patch_type;
            typedef typename field_for<T,_DIM>::type  axis_type;
            typedef box<T,_DIM>                       box_type;

            inline virtual ~curvilinear_mesh() throw() {}
            inline explicit curvilinear_mesh(const array<string> &names,
                                             const patch_type     full ) :
            mesh_info(DIM),patch_type(full),axis_handle()
            {
                setup(names);
            }

            inline axis_type       &X() throw()       { assert(dimension>=1); return *axis_handle[0]; }
            inline const axis_type &X() const throw() { assert(dimension>=1); return *axis_handle[0]; }

            inline axis_type       &Y() throw()       { assert(dimension>=2); return *axis_handle[1]; }
            inline const axis_type &Y() const throw() { assert(dimension>=2); return *axis_handle[1]; }

            inline axis_type       &Z() throw()       { assert(dimension>=3); return *axis_handle[2]; }
            inline const axis_type &Z() const throw() { assert(dimension>=3); return *axis_handle[2]; }

            inline void map_regular(const box_type &b, const patch_type inner)
            {
                // build a rectilinear mesh
                axis_names an; an.ensure(_DIM);
                for( field_db::iterator it=axis_db.begin();it != axis_db.end();++it)
                {
                    an.push_back( (**it).name );
                }
                rectilinear_mesh<T,_DIM> rmesh(an,*this);
                rmesh.map_regular(b,inner);
                transfer(rmesh,int2type<_DIM>());
            }

            inline void vtk( ios::ostream &fp ) const
            {
                fp << "DATASET STRUCTURED_GRID\n";
                fp << "DIMENSIONS";
                for(size_t dim=0;dim<DIM;++dim)
                {
                    fp(" %u", unsigned(__coord(this->width,dim)));
                }
                for(size_t dim=DIM;dim<3;++dim)
                {
                    fp  << " 1";
                }
                fp << '\n';
                fp("POINTS %u ",unsigned(this->items)); VTK::OutputScalarType<T>(fp); fp << '\n';
                output_vtk(fp,int2type<_DIM>());
            }

        protected:
            axis_type *axis_handle[_DIM];

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(curvilinear_mesh);
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

            inline void transfer( const rectilinear_mesh<T,_DIM> &rmesh, int2type<1> ) throw()
            {
                for(coord1D i=this->lower;i<=this->upper;++i)
                {
                    X()[i] = rmesh.X()[i];
                }
            }

            inline void transfer( const rectilinear_mesh<T,_DIM> &rmesh, int2type<2> ) throw()
            {
                for(coord1D j=this->lower.y;j<=this->upper.y;++j)
                {
                    for(coord1D i=this->lower.x;i<=this->upper.x;++i)
                    {
                        X()[j][i] = rmesh.X()[i];
                        Y()[j][i] = rmesh.Y()[j];
                    }
                }
            }

            inline void transfer( const rectilinear_mesh<T,_DIM> &rmesh, int2type<3> ) throw()
            {
                for(coord1D k=this->lower.z;k<=this->upper.z;++k)
                {
                    for(coord1D j=this->lower.y;j<=this->upper.y;++j)
                    {
                        for(coord1D i=this->lower.x;i<=this->upper.x;++i)
                        {
                            X()[k][j][i] = rmesh.X()[i];
                            Y()[k][j][i] = rmesh.Y()[j];
                            Z()[k][j][i] = rmesh.Z()[k];
                        }
                    }
                }
            }

            inline void output_vtk( ios::ostream &fp, int2type<1> ) const
            {
                for(coord1D i=this->lower;i<=this->upper;++i)
                {
                    fp("%.15g 0 0\n", X()[i]);
                }
            }

            inline void output_vtk( ios::ostream &fp, int2type<2> ) const
            {
                for(coord1D j=this->lower.y;j<=this->upper.y;++j)
                {
                    for(coord1D i=this->lower.x;i<=this->upper.x;++i)
                    {
                        fp("%.15g %.15g 0\n",X()[j][i],Y()[j][i]);
                    }
                }
            }

            inline void output_vtk( ios::ostream &fp, int2type<3> ) const
            {

                for(coord1D k=this->lower.z;k<=this->upper.z;++k)
                {
                    for(coord1D j=this->lower.y;j<=this->upper.y;++j)
                    {
                        for(coord1D i=this->lower.x;i<=this->upper.x;++i)
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

