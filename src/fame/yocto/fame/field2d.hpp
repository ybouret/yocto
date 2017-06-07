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
            
        private:
            YOCTO_DISABLE_ASSIGN(field2d);
            row   *rows;
            size_t nr;
            
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
                }

                //______________________________________________________________
                //
                // build rows
                rows -= this->outer.lower.y;
                try
                {
                    type                    *t_addr = this->entry;
                    const size_t             stride = this->outer.width.x;
                    

#if 0
                    const layout_of<coord1d> full_x = layout_ops::project(this->inner);
                    std::cerr << "self.inner=" << this->inner << std::endl;
                    std::cerr << "self.outer=" << this->outer << std::endl;
                    std::cerr << "self.depth=" << this->depth << std::endl;
                    for(coord1d y = this->outer.lower.y; y <= this->outer.upper.y; ++y, q += stride)
                    {
                        const coord1d rank1d = 0;
                        const coord1d size1d = 1;
                        const domain_of<coord1d> dom1d(rank1d,size1d,&size1d,full_x,this->pbc.x);
                        const string fid = this->name + ".row#" + vformat("%ld",y);
                        new (&rows[y]) row(fid,dom1d,this->depth,q);
                        std::cerr << "rows[" << y << "] inner=" << rows[y].inner << ", outer=" << rows[y].outer << std::endl;
                        ++nr;
                    }
#endif


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
