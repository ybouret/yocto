#ifndef YOCTO_SPADE_FIELD2D_INCLUDED
#define YOCTO_SPADE_FIELD2D_INCLUDED 1

#include "yocto/spade/field1d.hpp"

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

            //! build a field in 2 dimension
            /**
             \param L the layout
             \param G the ghosts
             \param E optional memory for rows+data=memory for one slice
             */
            inline explicit Field2D(const string      &id,
                                    const layout_type &L,
                                    const ghosts_type &G,
                                    void              *E = NULL ) :
            field_type(id,L,G),
            buflen(0),
            buffer(0),
            datjmp(ComputeDataOffset(this->outer)),
            rows(0),
            entry(0),
            shift(0)
            {
                if(!E)
                {
                    const size_t data_offset = datjmp;
                    const size_t data_length = this->outer.items   * sizeof(type);
                    buflen = memory::align(data_offset+data_length);
                    buffer = memory::kind<memory::global>::acquire(buflen);
                    try
                    {
                        build_on(buffer,G);
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
                    build_on(E,G);
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

            inline virtual const void *item_addr(const size_t indx) const throw()
            {
                assert(indx<this->outer.items);
                return &entry[indx];
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Field2D);
            size_t       buflen;
            void        *buffer;
            const size_t datjmp;
            row         *rows;
            type        *entry;
            row         *shift;

            inline void build_on(void *data,
                                 const ghosts_type &G)
            {
                std::cerr << "\tfield2D: linking rows..." << std::endl;
                const coord1D    nr = this->outer.width.y;
                const row_layout rowL(this->inner.lower.x,this->inner.upper.x);
                const row_ghost  row_lower_ghost(G.lower.peer.x,G.lower.size.x);
                const row_ghost  row_upper_ghost(G.upper.peer.x,G.upper.size.x);
                const row_ghosts rowG(G.rank,row_lower_ghost,row_upper_ghost);
                const size_t     stride = rowL.items;

                uint8_t *p  = static_cast<uint8_t *>(data);
                rows        = (row  *)( &p[0]     );
                entry       = (type *)( &p[datjmp]);
                shift       = rows-this->outer.lower.y;

                type *q = entry;
                coord1D     i=0;
                try
                {
                    const string id= this->name + "_row";
                    for(;i<nr;++i,q+=stride)
                    {
                        new ( &rows[i] ) row(id,rowL,rowG,q);
                    }
                }
                catch(...)
                {
                    while(i>=0)
                    {
                        rows[i].~row();
                        --i;
                    }
                    throw;
                }
            }


        };

    }
}

#endif
