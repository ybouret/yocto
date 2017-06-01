#ifndef YOCTO_SPADE_FIELD3D_INCLUDED
#define YOCTO_SPADE_FIELD3D_INCLUDED 1

#include "yocto/spade/field2d.hpp"

namespace yocto
{
    namespace spade
    {

        template <typename T>
        class Field3D : public field_of<T,coord3D>
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;
            //__________________________________________________________________
            //
            // 3D types
            //__________________________________________________________________
            typedef Layout3D            layout_type;
            typedef Ghosts3D            ghosts_type;
            typedef field_of<T,coord3D> field_type;

            //__________________________________________________________________
            //
            // 2D types
            //__________________________________________________________________
            //typedef Layout2D            slice_layout;
            //typedef Ghosts2D            slice_ghosts;
            //typedef Ghost2D             slice_ghost;
            typedef Field2D<T>          slice;
            typedef typename slice::row row;

            virtual ~Field3D() throw()
            {
                for(coord1D k=this->outer.width.z-1;k>=0;--k)
                {
                    destruct( &slices[k] );
                }
                memory::kind<memory::global>::release(buffer,buflen);
                slices=0;
                entry =0;
                shift =0;
            }

            explicit Field3D(const string      &id,
                             const layout_type &L,
                             const ghosts_type &G) :
            field_type(id,L,G),
            buflen(0),
            buffer(0),
            entry(0),
            slices(0),
            shift(0)
            {

                std::cerr << "3D outer: " << this->outer << std::endl;
                std::cerr << "3D inner: " << this->inner << std::endl;

                // computing 2D parameters
                const Layout2D slice_layout = layout_ops::extract(L);
                const Ghosts2D slice_ghosts = ghosts_ops::extract(G);

                // memory for slices
                const size_t   num_slices    = this->outer.width.z;
                const size_t   slices_offset = 0;
                const size_t   slices_length = num_slices * sizeof(slice);

                // memory for rows
                const size_t   rows_per_slice = this->outer.width.y;
                const size_t   num_rows       = num_slices * rows_per_slice;
                const size_t   rows_offset    = memory::align(slices_offset+slices_length);
                const size_t   rows_length    = num_rows * sizeof(row);

                // memory for data
                const size_t   data_offset    = memory::align(rows_offset+rows_length);
                const size_t   data_length    = this->outer.items * sizeof(type);

                std::cerr << "num_slices   =" << num_slices << "#" << sizeof(slice) << std::endl;
                std::cerr << "num_rows     =" << num_rows   << "#" << sizeof(typename slice::row) << std::endl;
                std::cerr << "slices_length=" << slices_length << "@" << slices_offset << std::endl;
                std::cerr << "rows_length  =" << rows_length << "@" << rows_offset << std::endl;
                std::cerr << "data_length  =" << data_length << "@" << data_offset << std::endl;

                // allocating
                buflen = memory::align(data_offset+data_length);
                buffer = memory::kind<memory::global>::acquire(buflen);

                const size_t items_by_slice = this->outer.width.x * this->outer.width.y;
                assert(items_by_slice*num_slices==this->outer.items);
                slices     = (slice *) _cast::shift(buffer,slices_offset);
                entry      = (type  *) _cast::shift(buffer,data_offset);
                shift      = slices - this->outer.lower.z;
                coord1D k = 0;
                try
                {
                    type *p = entry;
                    row  *r = (row   *) _cast::shift(buffer,rows_offset);
                    const string slice_name = this->name + "_slice";
                    for(;k<coord1D(num_slices);++k,p+=items_by_slice,r+=rows_per_slice)
                    {
                        new ( &slices[k] ) slice(slice_name,slice_layout,slice_ghosts,r,p);
                    }
                }
                catch(...)
                {
                    while(k>0)
                    {
                        destruct(&slices[k]);
                        --k;
                    }
                    throw;
                }


            }


            inline virtual const void *address_of( typename field_type::param_coord C ) const throw()
            {
                assert(this->outer.has(C));
                return &shift[C.z][C.y][C.x];
            }

            inline virtual const void *base() const throw()
            {
                return entry;
            }

            inline slice & operator[](const coord1D z) throw()
            {
                assert(z>=this->outer.lower.z);
                assert(z<=this->outer.upper.z);
                return shift[z];
            }

            inline const slice & operator[](const coord1D z) const throw()
            {
                assert(z>=this->outer.lower.z);
                assert(z<=this->outer.upper.z);
                return shift[z];
            }


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Field3D);
            size_t buflen;
            void  *buffer;
            type  *entry;
            slice *slices;
            slice *shift;
        };

        
        template <typename T>
        struct field_for<T,coord3D>
        {
            typedef Field3D<T> type;
        };
        
    }

}

#endif
