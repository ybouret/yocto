#ifndef YOCTO_IPSO_FIELD2D_INCLUDED
#define YOCTO_IPSO_FIELD2D_INCLUDED 1

#include "yocto/ipso/field1d.hpp"

namespace yocto
{
    namespace ipso
    {
        template <typename T>
        class field2D : public field<T>, public patch2D
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;
            typedef field1D<T> row;

#define YOCTO_IPSO_FIELD2D_CTOR(ID) \
field<T>(ID),                        \
patch2D(p),                           \
row_patch(p.lower.x,p.upper.x),        \
rows(0),                                \
wksp(0),                                 \
wlen(0)

            inline explicit field2D(const char    *id,
                                    const patch2D &p,
                                    void *usr_data=NULL,
                                    void *usr_rows=NULL) :
            YOCTO_IPSO_FIELD2D_CTOR(id)
            {
                build_with(usr_data,usr_rows);
            }

            inline explicit field2D(const string  &id,
                                    const patch2D &p,
                                    void *usr_data=NULL,
                                    void *usr_rows=NULL) :
            YOCTO_IPSO_FIELD2D_CTOR(*id)
            {
                build_with(usr_data,usr_rows);
            }


            inline virtual ~field2D() throw()
            {
                for(unit_t j=this->upper.y;j>=this->lower.y;--j)
                {
                    destruct( &rows[j] );
                }
                memory::kind<memory::global>::release(wksp,wlen);
            }

            inline row & operator[](const coord1D j) throw()
            {
                assert(j>=this->lower.y); assert(j<=this->upper.y);
                return rows[j];
            }

            inline const row & operator[](const coord1D j) const throw()
            {
                assert(j>=this->lower.y); assert(j<=this->upper.y);
                return rows[j];
            }


            const patch1D row_patch;

            template <typename U>
            inline void load( const field2D<U> &F, const patch2D &zone) throw()
            {
                assert(this->contains(zone));
                for(unit_t y=zone.upper.y;y>=zone.lower.y;--y)
                {
                    const row &peer_row = F[y];
                    row       &self_row = (*this)[y];
                    for(unit_t x=zone.upper.x;x>=zone.lower.y;--x)
                    {
                        self_row[x] = static_cast<T>(peer_row[x]);
                    }
                }
            }

            template <typename U>
            inline void save(  field2D<U> &F, const patch2D &zone) const throw()
            {
                assert(this->contains(zone));
                for(unit_t y=zone.upper.y;y>=zone.lower.y;--y)
                {
                    row       &peer_row = F[y];
                    const row &self_row = (*this)[y];
                    for(unit_t x=zone.upper.x;x>=zone.lower.y;--x)
                    {
                        peer_row[x] = static_cast<U>(self_row[x]);
                    }
                }
            }



        private:
            row   *rows; //!< allocated rows
            void  *wksp; //!< for this->items * sizeof(T) + width.y * sizeof(row)
            size_t wlen; //!< if allocated
            YOCTO_DISABLE_COPY_AND_ASSIGN(field2D);
            
            inline void link(void *data_addr,void *rows_addr)
            {
                assert(data_addr!=NULL); assert(rows_addr!=NULL);
                // prepare memory
                {
                    this->entry = (type *) data_addr;
                    rows        = (row  *) rows_addr;
                    rows       -= this->lower.y;
                }

                // link rows
                {
                    type *data = this->entry;
                    for(coord1D j=this->lower.y;j<=this->upper.y;++j,data+=this->width.x)
                    {
                        const string row_id = this->name + vformat("[%d]",int(j));
                        new (rows+j) row(row_id,row_patch,data);
                    }
                }
            }

            inline void build_with(void *usr_data,
                                   void *usr_rows)
            {
                assert(this->width.x==row_patch.width);
                const size_t data_offset = 0;
                const size_t data_length = this->items * sizeof(type);
                const size_t rows_offset = memory::align(data_offset+data_length);
                const size_t rows_length = this->width.y * sizeof(row);
                if(usr_data)
                {
                    link(usr_data,usr_rows);
                }
                else
                {
                    wlen = memory::align(rows_offset+rows_length);
                    wksp = memory::kind<memory::global>::acquire(wlen);
                    uint8_t *q = static_cast<uint8_t *>(wksp);
                    link(&q[data_offset],&q[rows_offset]);
                }
                this->set_bytes(this->items);
            }
        };

    }
}

#endif

