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
        public slots_of< field1d<T> >,
        public domain<COORD>,
        public real_indices
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;
            YOCTO_FAME_DECL_COORD;

            typedef layout<COORD>          layout_type;
            typedef domain<COORD>          domain_type;
            typedef domain<coord1d>        axis_domain;
            typedef field1d<T>             axis_type;
            typedef slots_of<axis_type>    holder_type;


            
            inline virtual ~rectilinear_mesh() throw() {}
            
            inline explicit rectilinear_mesh(const string      &tag,
                                             const domain_type &dom,
                                             const coord1d      num_ghosts) :
            mesh_info(tag),
            holder_type(DIMENSION),
            domain_type(dom),
            real_indices()
            {
                build_axis(num_ghosts);
            }

            template <typename U>
            inline explicit rectilinear_mesh(const string       &tag,
                                             const domain_type  &dom,
                                             const coord1d       num_ghosts,
                                             const box<U,COORD> &B) :
            mesh_info(tag),
            holder_type(DIMENSION),
            domain_type(dom),
            real_indices()
            {
                build_axis(num_ghosts);
                this->template map_to<U>(B);
            }

            //! regular mapping
            template <typename U>
            inline void map_to( const box<U,COORD> &B) throw()
            {
                const layout_type &f = this->full;
                for(size_t dim=0;dim<DIMENSION;++dim)
                {
                    axis_type    &a   = (*this)[dim];
                    const coord1d ilo = __coord(f.lower,dim);
                    const coord1d iup = __coord(f.upper,dim);
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

            inline void build_axis(const coord1d num_ghosts)
            {
                holder_type &self = *this;
                for(size_t dim=0;dim<DIMENSION;++dim)
                {
                    const axis_domain ax_dom = domain_ops::extract1(*this,dim);
                    const string      ax_tag = this->name + "_" + coord_info::axis_name(dim);
                    self.template append<string,const axis_domain &,coord1d>(ax_tag,ax_dom,num_ghosts);
                    imin[dim] = self[dim].imin[0];
                    imax[dim] = self[dim].imax[0];
                }
            }
        };
    }
}

#endif
