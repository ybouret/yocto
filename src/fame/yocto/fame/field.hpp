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
            const size_t item_size;       //!< bytes per item
            const size_t num_outer;       //!< total number of items, on outer layout
            const size_t num_inner;       //!< on inner layout, for information
            const size_t allocated_bytes; //!< owned bytes

            virtual ~field_data() throw();
            explicit field_data(const string &id,
                                const size_t  b_item,
                                const size_t  n_outer,
                                const size_t  n_inner);
            field_data(const field_data &other);
            
        protected:
            void set_allocated(const size_t bytes) throw();

        private:
            YOCTO_DISABLE_ASSIGN(field_data);
        };

        template <typename T,
        typename COORD>
        class field : public layouts<COORD>, public field_data
        {
        public:
            YOCTO_FAME_DECL_COORD;
            YOCTO_ARGUMENTS_DECL_T;

            typedef layouts<COORD> layouts_type;
            typedef layout<COORD>  layout_type;
            typedef domain<COORD>  domain_type;

            virtual type       &at(param_coord C) throw()       = 0;
            virtual const_type &at(param_coord C) const throw() = 0;

            inline void ld(param_type args)
            {
                for(size_t i=0;i<num_outer;++i)
                {
                    entry[i] = args;
                }
            }


        protected:
            inline explicit field(const string      &fid,
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

            field(const field &other) :
            layouts_type(other),
            field_data(other),
            count(0),
            entry(0)
            {
            }

            size_t count; //!< used for items of bytes

        public:
            type  *entry; //!< linear space of outer.items items...

        private:
            YOCTO_DISABLE_ASSIGN(field);
        };
    }
}

#endif
