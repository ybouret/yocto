#ifndef YOCTO_FAME_FIELD3D_INCLUDED
#define YOCTO_FAME_FIELD3D_INCLUDED 1

#include "yocto/fame/field2d.hpp"

namespace yocto
{

    namespace fame
    {

        template <typename T>
        class field3d : public field<T,coord3d>
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;
            typedef field<T,coord3d>                 field_type;
            typedef domain<coord3d>                  domain_type;
            typedef typename field_type::param_coord param_coord;
            typedef field2d<T>                       slice;
            typedef field1d<T>                       row;


            inline virtual ~field3d() throw()
            {
                clear();
            }

            inline explicit field3d(const string      &fid,
                                    const domain_type &dom,
                                    const coord1d      num_ghosts) :
            field_type(fid,dom,num_ghosts),
            slices(0),
            ns(0)
            {
                build();
            }

            inline slice & operator[](const coord1d z) throw()
            {
                assert(z>=this->outer.lower.z); assert(z<=this->outer.upper.z);
                return slices[z];
            }

            inline const slice & operator[](const coord1d z) const throw()
            {
                assert(z>=this->outer.lower.z); assert(z<=this->outer.upper.z);
                return slices[z];
            }

            inline virtual type & at(param_coord C) throw()
            {
                assert(this->outer.has(C));
                return slices[C.z][C.y][C.x];
            }

            inline virtual const_type & at(param_coord C) const throw()
            {
                assert(this->outer.has(C));
                return slices[C.z][C.y][C.x];
            }

            inline const slice & first() const throw()
            {
                return slices[this->outer.lower.z];
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(field3d);
            slice *slices;
            size_t ns;

            inline void build()
            {
                const size_t num_slices     = this->outer.width.z;
                const size_t slices_offset  = 0;
                const size_t slices_length  = num_slices * sizeof(slice);

                const size_t rows_per_slice = this->outer.width.y;
                const size_t num_rows       = num_slices * rows_per_slice;
                const size_t rows_offset    = memory::align(slices_offset+slices_length);
                const size_t rows_length    = num_rows * sizeof(row);

                const size_t num_data       = this->outer.items;
                const size_t data_offset    = memory::align(rows_offset+rows_length);
                const size_t data_length    = num_data * sizeof(T);

                this->count = memory::align(data_offset + data_length);
                uint8_t *p  = (uint8_t *)memory::kind<memory::global>::acquire(this->count);
                slices      = (slice *) &p[slices_offset];
                row  *r     = (row   *) &p[rows_offset  ];
                this->entry = (type  *) &p[data_offset  ];

                slices -= this->outer.lower.z;
                type *t = this->entry;
                const size_t data_per_slice = this->outer.width.x * this->outer.width.y;
                try
                {
                    // create the sub-domain 2D
                    const layout<coord2d>  full_xy  = layout_ops::project(this->full);
                    const coord2d          ranks_xy(this->self.ranks.x,this->self.ranks.y);
                    const coord2d          sizes_xy(this->full.sizes.x,this->full.sizes.y);
                    split<coord2d>         split_xy(full_xy,sizes_xy.__prod(),&sizes_xy);
                    const coord1d          rank_xy = split_xy.global_rank(ranks_xy);
                    const coord1d          size_xy = split_xy.size;
                    const coord2d          pbc_xy(this->pbc.x,this->pbc.y);
                    const coord1d          ng  = this->depth;
                    const domain<coord2d>  dom_xy(rank_xy,size_xy,&sizes_xy,full_xy,pbc_xy);

                    for(coord1d z=this->outer.lower.z;z<=this->outer.upper.z;++z,r+=rows_per_slice,t+=data_per_slice)
                    {
                        const string slice_id = this->name + ".slice#" + vformat("%ld",long(z));
                        new (&slices[z]) slice(slice_id,dom_xy,ng,r,t);
                        ++ns;
                    }
                    assert(num_slices==ns);
                }
                catch(...)
                {
                    clear();
                    throw;
                }

            }

            inline void clear() throw()
            {
                assert(slices!=NULL);
                slices += this->outer.lower.z;
                while(ns>0)
                {
                    --ns;
                    slices[ns].~slice();
                }
                if(this->count)
                {
                    void *p = slices;
                    memory::kind<memory::global>::release(p,this->count);
                }
            }

        };


        template <typename T>
        struct field_for<T,coord3d>
        {
            typedef field3d<T> type;
        };
    }
}

#endif
