#ifndef YOCTO_IPSO_MESH_RECTILINEAR_INCLUDED
#define YOCTO_IPSO_MESH_RECTILINEAR_INCLUDED 1

#include "yocto/ipso/mesh.hpp"

namespace yocto
{
    namespace ipso
    {
        template <typename T,const size_t DIM>
        class rectilinear_mesh :
        public mesh_info,
        public patch_for<DIM>::type
        {
        public:
            typedef typename patch_for<DIM>::type patch_type;
            typedef patch1D                       axis_patch;
            typedef field1D<T>                    axis_type;

            inline virtual ~rectilinear_mesh() throw() {}
            inline explicit rectilinear_mesh( const array<string> &names, const patch_type full ) :
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

        protected:
            axis_type *axis_handle[DIM];

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(rectilinear_mesh);
            void setup(const array<string> &names)
            {
                assert(names.size()==DIM);
                const coord1D *lo = (const coord1D *) &(this->lower);
                const coord1D *up = (const coord1D *) &(this->upper);

                for(size_t dim=0;dim<DIM;++dim)
                {
                    const axis_patch p(lo[dim],up[dim]);
                    axis_type       &axis = axis_db.build<axis_type>(names[dim+1],p);
                    axis_handle[dim] = &axis;
                    axis_info.append(axis);
                }
            }
        };
    }
}

#endif

