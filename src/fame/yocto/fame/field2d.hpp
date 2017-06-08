#ifndef YOCTO_FAME_FIELD2D_INCLUDED
#define YOCTO_FAME_FIELD2D_INCLUDED 1

#include "yocto/fame/field1d.hpp"

namespace yocto
{
    
    namespace fame
    {
        
        template <typename T>
        class field2d : public field<T,coord2d>
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;
            typedef field<T,coord2d>                 field_type;
            typedef domain<coord2d>                  domain_type;
            typedef typename field_type::param_coord param_coord;
            typedef field1d<T>                       row;
            
            explicit field2d(const string      &fid,
                             const domain_type &dom,
                             const coord1d      num_ghosts,
                             void              *rows_entry=0,
                             void              *user_entry=0) :
            field_type(fid,dom,num_ghosts),
            rows(0),
            nr(0)
            {
                build_from(rows_entry,user_entry);
            }


            inline field2d(const string &fid, const field2d &other) :
            field_type(fid,other,other.depth),
            rows(0),
            nr(0)
            {
                build_from(NULL,NULL);
                assert(this->num_outer==other.num_outer);
                memcpy(this->entry,other.entry,this->num_outer*sizeof(T));
            }
            

            inline virtual ~field2d() throw()
            {
                clear();
            }
            
            inline row & operator[](const coord1d y) throw()
            {
                return rows[y];
            }
            
            inline const row & operator[](const coord1d y) const throw()
            {
                return rows[y];
            }
            
            inline virtual type & at(param_coord C) throw()
            {
                assert(this->outer.has(C));
                return rows[C.y][C.x];
            }
            
            inline virtual const_type & at(param_coord C) const throw()
            {
                assert(this->outer.has(C));
                return rows[C.y][C.x];
            }

            inline const row & first() const throw()
            {
                return rows[this->outer.lower.y];
            }

            inline void run_hash( hashing::function &h ) const throw()
            {
                
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(field2d);
            row   *rows; //!< the rows
            size_t nr;   //!< internal built rows counter
            
            inline void clear() throw()
            {
                rows += this->outer.lower.y;
                while(nr>0)
                {
                    rows[--nr].~row();
                }
                if(this->count)
                {
                    void *p = rows;
                    memory::kind<memory::global>::release(p,this->count);
                }
                rows = 0;
                
            }
            
            inline void build_from(void *rows_entry,void *user_entry)
            {

                //______________________________________________________________
                //
                // prepare memory
                //______________________________________________________________
                if(rows_entry)
                {
                    assert(user_entry!=NULL);
                    rows        = (row  *)rows_entry;
                    this->entry = (type *)user_entry;
                }
                else
                {
                    const size_t rows_offset = 0;
                    const size_t num_rows    = this->outer.width.y;
                    const size_t rows_length = sizeof(row) * num_rows;
                    
                    const size_t items_offset = memory::align(rows_offset+rows_length);
                    const size_t items_length = this->num_outer * sizeof(T);
                    
                    this->count = memory::align(items_offset+items_length);
                    uint8_t  *p = (uint8_t *) memory::kind<memory::global>::acquire(this->count);
                    rows        = (row  *)&p[rows_offset];
                    this->entry = (type *)&p[items_offset];

                    this->set_allocated(this->count);
                }

                //______________________________________________________________
                //
                // build rows
                //______________________________________________________________
                rows -= this->outer.lower.y;
                try
                {
                    // data walking
                    type                    *t_addr = this->entry;
                    const size_t             stride = this->outer.width.x;

                    // preparing sub-domain
                    const layout<coord1d>    full_x = layout_ops::project(this->full);
                    const coord1d            rank_x = this->self.ranks.x;
                    const coord1d            size_x = this->full.sizes.x;
                    const coord1d            pbc_x  = this->pbc.x;
                    const coord1d            ng     = this->depth;
                    const domain<coord1d>    dom_x(rank_x,size_x,&size_x,full_x,pbc_x);

                    // build each row
                    for(coord1d y = this->outer.lower.y; y <= this->outer.upper.y; ++y, t_addr += stride)
                    {
                        const string fid = this->name + ".row#" + vformat("%ld",long(y));
                        new (&rows[y]) row(fid,dom_x,ng,t_addr);
                        ++nr;
                    }

                    
                }
                catch(...)
                {
                    clear();
                    throw;
                }
            }
            
        };
        
    }
    
}

#endif
