#ifndef YOCTO_FAME_MESH_CURVILINEAR_INCLUDED
#define YOCTO_FAME_MESH_CURVILINEAR_INCLUDED 1

#include "yocto/fame/mesh.hpp"
#include "yocto/fame/layouts.hpp"

namespace yocto
{
    namespace fame
    {
        template <typename T,typename COORD>
        class curvilinear_mesh :
        public mesh_info,
        public slots_of< typename field_for<T,COORD>::type >,
        public domain<COORD>
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;
            YOCTO_FAME_DECL_COORD;

            typedef layout<COORD>                     layout_type;
            typedef domain<COORD>                     domain_type;
            typedef typename field_for<T,COORD>::type coords_type;
            typedef slots_of<coords_type>             holder_type;

            inline virtual ~curvilinear_mesh() throw() {}

            inline explicit curvilinear_mesh(const string      &tag,
                                             const domain_type &dom,
                                             const coord1d      num_ghosts) :
            mesh_info(tag),
            holder_type(DIMENSION),
            domain_type(dom)
            {
                build_coords(num_ghosts);
            }


            //! regular mapping
            template <typename U>
            inline void map_to(const box<U,COORD> &B) throw()
            {
#if 0
                const layout_type &f = this->full;
                for(size_t dim=0;dim<DIMENSION;++dim)
                {
                    coords_type   &a   = (*this)[dim];
                    const coord1d ilo = __coord(f.lower,dim);
                    const coord1d iup = __coord(f.upper,dim);
                    const coord1d del = iup-ilo;
                    const_type    l   = type(B.__lower()[dim]);
                    const_type    w   = type(B.__width()[dim]);
                    const_type    u   = type(B.__upper()[dim]);
                    if(del<=0)
                    {
                        //const_type v = type( (u+l)/2 );
                        //a[ilo] = (u+l)/2;
                    }
                    else
                    {
                        const coord1d ini = __coord(a.outer.lower,dim);
                        const coord1d end = __coord(a.outer.upper,dim);
                        for(coord1d i = ini; i <= end; ++i)
                        {
                            //const_type v = type(l + ( (i-ilo)*w )/del);
                            for(size_t djm=0;djm<DIMENSION;++djm)
                            {
                                //(void)v;
                            }
                        }
                    }
                }
#endif
            }


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(curvilinear_mesh);
            inline void build_coords(const coord1d num_ghosts)
            {
                holder_type &hld = *this;
                for(size_t dim=0;dim<DIMENSION;++dim)
                {
                    const string coords_tag = this->name + "_" + coord_info::axis_name(dim);
                    hld. template append<const string&,const domain_type&,coord1d>(coords_tag,*this,num_ghosts);
                    assert(1+dim==hld.size);
                }
            }

        };
        
    }
    
}

#endif
