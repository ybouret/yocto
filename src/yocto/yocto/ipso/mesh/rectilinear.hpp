#ifndef YOCTO_IPSO_MESH_RECTILINEAR_INCLUDED
#define YOCTO_IPSO_MESH_RECTILINEAR_INCLUDED 1

#include "yocto/ipso/mesh.hpp"
#include "yocto/ipso/box.hpp"
#include "yocto/ipso/format/vtk.hpp"

namespace yocto
{
    namespace ipso
    {
        template <typename T,const size_t DIMENSION>
        class rectilinear_mesh :
        public mesh_info
        {
        public:
            static const size_t DIM = DIMENSION;
            YOCTO_ARGUMENTS_DECL_T;
            typedef typename coord_for<DIMENSION>::type coord_type;
            typedef subset<coord_type>                  subset_type;
            typedef field1D<T>                          axis_type;
            typedef box<T,DIMENSION>                    box_type;
            
            //! dtor
            inline virtual ~rectilinear_mesh() throw() {}
            
            //! default constructor, based on a precomputed subset
            inline explicit rectilinear_mesh(const string        &id,
                                             const array<string> &names,
                                             const subset_type   &sub) :
            mesh_info(id,DIMENSION),
            axis_handle()
            {
                build_subsets1D_from(sub,subs,true);
                setup(names);
            }

            inline explicit rectilinear_mesh(const char          *id,
                                             const array<string> &names,
                                             const subset_type   &sub) :
            mesh_info(id,DIMENSION),
            axis_handle()
            {
                build_subsets1D_from(sub,subs,true);
                setup(names);
            }
            
            inline axis_type       &X() throw()       { assert(dimension>=1); return *axis_handle[0]; }
            inline const axis_type &X() const throw() { assert(dimension>=1); return *axis_handle[0]; }
            
            inline axis_type       &Y() throw()       { assert(dimension>=2); return *axis_handle[1]; }
            inline const axis_type &Y() const throw() { assert(dimension>=2); return *axis_handle[1]; }
            
            inline axis_type       &Z() throw()       { assert(dimension>=3); return *axis_handle[2]; }
            inline const axis_type &Z() const throw() { assert(dimension>=3); return *axis_handle[2]; }
            
            inline void map_regular(const box_type         &b,
                                    const patch<coord_type> inner)
            {
                const coord1D * _ilo  = (const coord1D *) &(inner.lower);
                const coord1D * _iup  = (const coord1D *) &(inner.upper);
                const_type    * _vmin = (const_type *)(&b.lower);
                const_type    * _dv   = (const_type *)(&b.width);
                for(size_t dim=0;dim<DIM;++dim)
                {
                    axis_type    &a    = *axis_handle[dim];
                    const coord1D ilo  = _ilo[dim];
                    const coord1D iup  = _iup[dim];
                    if(ilo>=iup) throw exception("rectilinear_mesh.map_regular(bad inner %s)", __coord_name(dim) );
                    const coord1D den  = iup - ilo;
                    const_type    vmin = _vmin[dim];
                    const_type    dv   = _dv[dim];
                    const coord1D slo  = a.lower;
                    const coord1D sup  = a.upper;
                    for(coord1D i=slo;i<=sup;++i)
                    {
                        a[i] = vmin + ( (i-ilo) * dv ) / den;
                    }
                }
            }

            //! after a VTK::Header
            inline void vtk( ios::ostream &fp ) const
            {
                fp << "DATASET RECTILINEAR_GRID\n";
                fp << "DIMENSIONS";
                for(size_t dim=0;dim<DIM;++dim)
                {
                    fp(" %u", unsigned(axis_handle[dim]->width));
                }
                for(size_t dim=DIM;dim<3;++dim)
                {
                    fp  << " 1";
                }
                fp << "\n";
                for(size_t dim=0;dim<DIM;++dim)
                {
                    fp << mesh_info::vtk_coordinates(dim);
                    const axis_type &a = *axis_handle[dim];
                    fp(" %u ", unsigned(a.width));
                    VTK::OutputScalarType<T>(fp);
                    fp << '\n';
                    for(coord1D i=a.lower;i<=a.upper;++i)
                    {
                        fp("%.15g",a[i]);
                        if(i<a.upper) fp << ' ';
                    }
                    fp << '\n';
                }
                for(size_t dim=DIM;dim<3;++dim)
                {
                    fp << mesh_info::vtk_coordinates(dim) << " 1 ";
                    VTK::OutputScalarType<T>(fp); fp << '\n';
                    fp << "0\n";
                }
                
            }
            
        protected:
            axis_type *axis_handle[DIMENSION];
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(rectilinear_mesh);
            subset<coord1D>::list subs;

            inline void setup( const array<string> &names )
            {
                assert(names.size()==DIMENSION);
                assert(subs.size   ==DIMENSION);
                memset(axis_handle,0,sizeof(axis_handle));
                
                subset<coord1D> *sub = subs.head;
                for(size_t dim=0;dim<DIM;++dim,sub=sub->next)
                {
                    axis_type       &axis = axis_db.build<axis_type>(names[dim+1],sub->outer);
                    axis_handle[dim] = &axis;
                    axis_info.append(axis);
                    sub->allocate_swaps_for( sizeof(T) );
                }
            }
        };
        
    }
}

#endif

