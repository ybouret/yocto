#ifndef YOCTO_FAME_FIELD1D_INCLUDED
#define YOCTO_FAME_FIELD1D_INCLUDED 1

#include "yocto/fame/field.hpp"
#include "yocto/code/bmove.hpp"

namespace yocto
{

    namespace fame
    {

        template <typename T>
        class field1d : public field<T,coord1d>
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;
            typedef field<T,coord1d>                 field_type;
            typedef domain<coord1d>                  domain_type;
            typedef typename field_type::param_coord param_coord;

            inline explicit field1d(const string      &fid,
                                    const domain_type &dom,
                                    const coord1d      num_ghosts = 0,
                                    void  *            user_entry = 0) :
            field_type(fid,dom,num_ghosts),
            shift(0)
            {
                build_from(user_entry);
            }

            //! copy with name change
            inline field1d(const string  &fid,
                           const field1d &other) :
            field_type(fid,other,other.depth),
            shift(0)
            {
                build_from(NULL);
                assert(this->num_outer==other.num_outer);
                memcpy(this->entry,other.entry,this->num_outer*sizeof(T));
            }

            inline virtual  ~field1d() throw()
            {
                if(this->count)
                {
                    memory::kind<memory::global>::release_as<type>(this->entry,this->count);
                }
                shift = 0;
            }

            inline virtual type & at( param_coord C) throw()
            {
                assert(this->outer.has(C));
                return shift[C];
            }

            inline virtual const_type & at( param_coord C) const throw()
            {
                assert(this->outer.has(C));
                return shift[C];
            }

            inline type & operator[](const coord1d C) throw()
            {
                assert(this->outer.has(C));
                return shift[C];
            }

            inline const_type & operator[](const coord1d C) const throw()
            {
                assert(this->outer.has(C));
                return shift[C];
            }


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(field1d);
            type *shift;
            inline void build_from(void *user_entry)
            {
                if(user_entry)
                {
                    this->entry = (type *)user_entry;
                }
                else
                {
                    this->count = this->num_outer;
                    this->entry = memory::kind<memory::global>::acquire_as<type>(this->count);
                    this->set_allocated(this->count*sizeof(T));
                }
                shift = this->entry - this->outer.lower;
            }
            
            virtual void save(const layout<coord1d> &sub, void *buf ) const throw()
            {
                assert(this->outer.contains(sub));
                uint8_t *p = (uint8_t *)buf;
                for(coord1d i=sub.lower;i<=sub.upper;++i)
                {
                    core::bmove<sizeof(T)>(p,&(*this)[i]);
                    p += sizeof(T);
                }
            }
            
            virtual void load(const layout<coord1d> &sub, const void *buf ) throw()
            {
                const uint8_t *p = (const uint8_t *)buf;
                for(coord1d i=sub.lower;i<=sub.upper;++i)
                {
                    core::bmove<sizeof(T)>(&(*this)[i],p);
                    p += sizeof(T);
                }

            }

        };

        template <typename T>
        struct field_for<T,coord1d>
        {
            typedef field1d<T> type;
        };
    }

}

#endif
