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

            virtual ~field1D() throw()
            {
                if(count)
                {
                    memory::kind<memory::global>::release_as<type>(entry,count);
                    shift = 0;
                    (size_t &)(this->owned_memory) = 0;
                }
            }

            explicit field1D(coord1D lo,
                             coord1D up,
                             coord1D ghost_lo  = coord_info<coord1D>::zero,
                             coord1D ghost_up  = coord_info<coord1D>::zero,
                             type   *usr_entry = 0) :
            field_type(lo,up,ghost_lo,ghost_up),
            count(0),
            entry((type *)usr_entry),
            shift(entry-this->lower)
            {
                if(!usr_entry)
                {
                    count  = this->items;
                    entry  = memory::kind<memory::global>::acquire_as<type>(count);
                    shift  = entry - this->lower;
                    (size_t &)(this->owned_memory) = count * sizeof(T);
                }
            }

            const void *address_of( typename field_type::param_coord C ) const throw()
            {
                assert(this->has(C));
                return shift+C;
            }

            inline type & operator[]( coord1D C ) throw()
            {
                assert(this->has(C));
                return shift[C];
            }

            inline const_type & operator[]( coord1D C ) const throw()
            {
                assert(this->has(C));
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
