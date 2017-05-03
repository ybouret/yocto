#ifndef YOCTO_SPADE_FIELD1D_INCLUDED
#define YOCTO_SPADE_FIELD1D_INCLUDED 1

#include "yocto/spade/field.hpp"

namespace yocto
{
    namespace spade
    {
        template <typename T>
        class field1D : public field_of<T,coord1D>
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;
            typedef field_of<T,coord1D> field_type;
            typedef layout1D            layout_type;
            typedef ghosts<coord1D>     ghosts_type;

            virtual ~field1D() throw()
            {
                if(count)
                {
                    memory::kind<memory::global>::release_as<type>(entry,count);
                    shift = 0;
                    this->set_bytes(0);
                }
            }

            explicit field1D(const layout_type &L,
                             const ghosts_type &G,
                             type              *E = NULL):
            field_type(L,G),
            count(0),
            entry((type *)E),
            shift(entry-this->outer.lower)
            {
                if(!E)
                {
                    count  = this->outer.items;
                    entry  = memory::kind<memory::global>::acquire_as<type>(count);
                    shift  = entry - this->outer.lower;
                    this->set_bytes(count * sizeof(T));
                }
            }

            const void *address_of( typename field_type::param_coord C ) const throw()
            {
                assert(this->outer.has(C));
                return shift+C;
            }

            inline type & operator[]( coord1D C ) throw()
            {
                assert(this->outer.has(C));
                return shift[C];
            }

            inline const_type & operator[]( coord1D C ) const throw()
            {
                assert(this->outer.has(C));
                return shift[C];
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(field1D);
            size_t count;
            type  *entry;
            type  *shift;

        };

    }
}

#endif
