#ifndef YOCTO_FAME_FIELD_INCLUDED
#define YOCTO_FAME_FIELD_INCLUDED 1

#include "yocto/fame/layouts.hpp"
#include "yocto/string.hpp"

namespace yocto
{
    namespace fame
    {
        class field_data
        {
        public:
            const string name;
            const size_t item_size;
            const size_t num_outer;
            const size_t num_inner;

            virtual ~field_data() throw();
            explicit field_data(const string &id,
                                const size_t  b_item,
                                const size_t  n_outer,
                                const size_t  n_inner);
            field_data(const field_data &other);

        private:
            YOCTO_DISABLE_ASSIGN(field_data);
        };

        template <typename T,
        typename COORD>
        class field_of : public layouts_of<COORD>, public field_data
        {
        public:
            YOCTO_FAME_DECL_COORD;
            YOCTO_ARGUMENTS_DECL_T;

            typedef layouts_of<COORD> layouts_type;
            typedef layout_of<COORD>  layout_type;
            typedef domain_of<COORD>  domain_type;

            virtual type       &at(param_coord C) throw()       = 0;
            virtual const_type &at(param_coord C) const throw() = 0;

        protected:
            inline explicit field_of(const string      &fid,
                                     const domain_type &dom,
                                     const coord1d      num_ghosts) :
            layouts_type(dom,num_ghosts),
            field_data(fid,
                       sizeof(type),
                       this->outer.items,
                       this->inner.items),
            count(0),
            entry(0)
            {
            }

            size_t count; //!< used for items of bytes
            type  *entry; //!< linear space of outer.items items...

            field_of(const field_of &other) :
            layouts_type(other),
            field_data(other),
            count(0),
            entry(0)
            {
            }
            
        private:
            YOCTO_DISABLE_ASSIGN(field_of);
        };
    }
}

#endif
