#ifndef YOCTO_SPADE_FIELD1D_INCLUDED
#define YOCTO_SPADE_FIELD1D_INCLUDED 1

#include "yocto/spade/field.hpp"

namespace yocto
{
    namespace spade
    {
        template <typename T>
        class Field1D : public field_of<T,coord1D>
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;
            typedef field_of<T,coord1D> field_type;
            typedef Layout1D            layout_type;
            typedef Ghosts<coord1D>     ghosts_type;

            virtual ~Field1D() throw()
            {
                if(count)
                {
                    memory::kind<memory::global>::release_as<type>(entry,count);
                    this->set_allocated(0);
                }
                shift = 0;
            }

            //! build a field
            /**
             \param L a layout
             \param G some ghosts decription
             \param E optional entry with size >= outer.items*sizeof(T)
             */
            explicit Field1D(const string      &id,
                             const layout_type &L,
                             const ghosts_type &G,
                             void              *E = NULL):
            field_type(id,L,G),
            count(0),
            entry((type *)E),
            shift(entry-this->outer.lower)
            {
                if(!E)
                {
                    count  = this->outer.items;
                    entry  = memory::kind<memory::global>::acquire_as<type>(count);
                    shift  = entry - this->outer.lower;
                    this->set_allocated(count * sizeof(T));
                }
            }

            const void *address_of( typename field_type::param_coord C ) const throw()
            {
                assert(this->outer.has(C));
                return shift+C;
            }

            inline virtual type & operator[]( const coord1D  C) throw()
            {
                assert(this->outer.has(C));
                return shift[C];
            }

            inline const_type & operator[](  const coord1D  C ) const throw()
            {
                assert(this->outer.has(C));
                return shift[C];
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Field1D);
            size_t count;
            type  *entry;
            type  *shift;

        };

    }
}

#endif
