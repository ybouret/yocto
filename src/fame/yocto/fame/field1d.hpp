#ifndef YOCTO_FAME_FIELD1D_INCLUDED
#define YOCTO_FAME_FIELD1D_INCLUDED 1

#include "yocto/fame/field.hpp"

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
                                    const coord1d      num_ghosts,
                                    void  *            user_entry = 0) :
            field_type(fid,dom,num_ghosts),
            shift(0)
            {
                build_from(user_entry);
            }

            inline field1d(const field1d &other) :
            field_type(other)
            {
                build_from(NULL);
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
            YOCTO_DISABLE_ASSIGN(field1d);
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
                }
                shift = this->entry - this->outer.lower;
            }

        };

    }

}

#endif
