#ifndef YOCTO_SPADE_FIELD2D_INCLUDED
#define YOCTO_SPADE_FIELD2D_INCLUDED 1

#include "yocto/spade/field1d.hpp"
#include "yocto/code/cast.hpp"

namespace yocto
{
    namespace spade
    {

        template <typename T>
        class Field2D : public field_of<T,coord2D>
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;
            //__________________________________________________________________
            //
            // 2D types
            //__________________________________________________________________
            typedef Layout2D            layout_type;
            typedef Ghosts2D            ghosts_type;
            typedef field_of<T,coord2D> field_type;

            //__________________________________________________________________
            //
            // 1D types
            //__________________________________________________________________
            typedef Field1D<T>          row;
            typedef Layout1D            row_layout;
            typedef Ghost1D             row_ghost;
            typedef Ghosts1D            row_ghosts;


#if 0
            //! compute data offset
            static inline
            size_t ComputeDataOffset(const Layout2D &Outer) throw()
            {
                return memory::align(Outer.width.y * sizeof(row));
            }

            //! size in bytes for one slice
            static inline
            size_t ComputeBufferSize(const Layout2D &Outer) throw()
            {
                const size_t data_offset = ComputeDataOffset(Outer);
                const size_t data_length = Outer.items * sizeof(type);
                return memory::align(data_offset+data_length);
            }
#endif

            //! build a field in 2 dimension
            /**
             \param L the layout
             \param G the ghosts
             \param E optional memory for rows+data=memory for one slice
             */
            inline explicit Field2D(const string      &id,
                                    const layout_type &L,
                                    const ghosts_type &G,
                                    void              *rows_wksp = NULL,
                                    void              *data_wksp = NULL) :
            field_type(id,L,G),
            buflen(0),
            buffer(0),
            rows(0),
            entry(0),
            shift(0)
            {
                if(!rows_wksp)
                {
                    assert(!data_wksp);
                    const size_t nr          = this->outer.width.y;
                    const size_t rows_offset = 0;
                    const size_t rows_length = nr * sizeof(row);
                    const size_t data_offset = memory::align(rows_offset+rows_length);
                    const size_t data_length = this->outer.items   * sizeof(type);
                    buflen = memory::align(data_offset+data_length);
                    buffer = memory::kind<memory::global>::acquire(buflen);

                    try
                    {
                        rows_wksp = _cast::shift(buffer,rows_offset);
                        data_wksp = _cast::shift(buffer,data_offset);
                        build_on(rows_wksp,data_wksp,G);
                    }
                    catch(...)
                    {
                        memory::kind<memory::global>::release(buffer,buflen);
                        throw;
                    }
                    this->set_allocated(buflen);
                }
                else
                {
                    build_on(rows_wksp,data_wksp,G);
                }
            }

            inline virtual ~Field2D() throw()
            {
                for(coord1D i=this->outer.width.y-1;i>=0;--i)
                {
                    rows[i].~row();
                }
                if(buflen)
                {
                    memory::kind<memory::global>::release(buffer,buflen);
                    rows  = 0;
                    entry = 0;
                    shift = 0;
                }
            }

            inline virtual const void *address_of( typename field_type::param_coord C ) const throw()
            {
                assert(this->outer.has(C));
                return &shift[C.y][C.x];
            }

            inline row & operator[](const coord1D y) throw()
            {
                assert(y>=this->outer.lower.y);
                assert(y<=this->outer.upper.y);
                return shift[y];
            }

            inline const row & operator[](const coord1D y) const throw()
            {
                assert(y>=this->outer.lower.y);
                assert(y<=this->outer.upper.y);
                return shift[y];
            }

            inline virtual const void *base() const throw()
            {
                return entry;
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Field2D);
            size_t       buflen;
            void        *buffer;
            row         *rows;
            type        *entry;
            row         *shift;

            inline void build_on(void *rows_wksp,
                                 void *data_wksp,
                                 const ghosts_type &G)
            {
                //______________________________________________________________
                //
                // computing 1D parameters
                //______________________________________________________________

                const coord1D    r_count       = this->outer.width.y;
                const row_layout r_layout      = layout_ops::extract(this->inner,0);
                const row_ghosts r_ghosts      = ghosts_ops::extract(G,0);
                const size_t     items_per_row = this->outer.width.x;

                //______________________________________________________________
                //
                // assigning memory
                //______________________________________________________________
                rows        = (row  *)(rows_wksp);
                entry       = (type *)(data_wksp);
                shift       = rows-this->outer.lower.y;

                //______________________________________________________________
                //
                // linking with memory check
                //______________________________________________________________
                type   *q = entry;
                coord1D i = 0;
                try
                {
                    const string id= this->name + "_row";
                    for(;i<r_count;++i,q+=items_per_row)
                    {
                        new ( &rows[i] ) row(id,r_layout,r_ghosts,q);
                    }
                }
                catch(...)
                {
                    while(i>=0)
                    {
                        destruct(&rows[i]);
                        --i;
                    }
                    throw;
                }
            }


        };
        
        template <typename T>
        struct field_for<T,coord2D>
        {
            typedef Field2D<T> type;
        };

    }
}

#endif
