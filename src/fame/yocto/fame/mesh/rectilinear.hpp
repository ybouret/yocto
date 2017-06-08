#ifndef YOCTO_FAME_MESH_RECTILINEAR_INCLUDED
#define YOCTO_FAME_MESH_RECTILINEAR_INCLUDED 1

#include "yocto/fame/mesh.hpp"
#include "yocto/fame/layouts.hpp"

namespace yocto
{
    namespace fame
    {
        template <typename T,typename COORD>
        class rectilinear_mesh :
        public mesh_info,
        public slots_of< field1d<T> >
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;
            YOCTO_FAME_DECL_COORD;

            typedef layout<COORD>          layout_type;
            typedef domain<COORD>          domain_type;
            typedef field1d<T>             axis_type;
            typedef slots_of<axis_type>    base_type;
            
            
            
            inline virtual ~rectilinear_mesh() throw() {}
            
            inline explicit rectilinear_mesh(const string      &id,
                                             const domain_type &dom,
                                             const coord1d      num_ghosts) :
            mesh_info(id),
            base_type(DIMENSION)
            {
                base_type &self = *this;
                for(size_t dim=0;dim<DIMENSION;++dim)
                {
                    const domain<coord1d> dom1 = domain_ops::extract1(dom,dim);
                    const string          axID = this->name + "_" + coord_info::axis_name(dim);
                    self.template append<string,const domain<coord1d> &,coord1d>(axID,dom1,num_ghosts);
                }
                
            }
            
            //! regular mapping
            template <typename U>
            inline void map_to( const box<U,COORD> &B, const layout_type &full ) throw()
            {
                for(size_t dim=0;dim<DIMENSION;++dim)
                {
                    axis_type    &a   = (*this)[dim];
                    const coord1d ilo = __coord(full.lower,dim);
                    const coord1d iup = __coord(full.upper,dim);
                    const coord1d del = iup-ilo;
                    const_type    l = type(B.__lower()[dim]);
                    const_type    w = type(B.__width()[dim]);
                    const_type    u = type(B.__upper()[dim]);
                    if(del<=0)
                    {
                        a[ilo] = (u+l)/2;
                    }
                    else
                    {
                        for(coord1d i = a.outer.lower; i <= a.outer.upper; ++i)
                        {
                            
                            a[i] = l + ( (i-ilo)*w )/del;
                        }
                    }
                }
            }


        private:
            YOCTO_DISABLE_ASSIGN(rectilinear_mesh);
        };
    }
}

#endif
