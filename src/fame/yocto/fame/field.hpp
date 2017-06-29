#ifndef YOCTO_FAME_FIELD_INCLUDED
#define YOCTO_FAME_FIELD_INCLUDED 1

#include "yocto/fame/layouts.hpp"
#include "yocto/string.hpp"
#include "yocto/ptr/intr.hpp"

namespace yocto
{
    namespace fame
    {
        class field_data : public counted_object
        {
        public:
            typedef intr_ptr<string,field_data> pointer;

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
            const string & key() const throw();

            inline const void *address_of( const coord1d offset_value ) const throw()
            {
                return __get(offset_value);
            }

            inline void *address_of(const coord1d offset_value ) throw()
            {
                return (void *) __get(offset_value);
            }

            static bool are_compatible(const field_data &lhs, const field_data &rhs) throw();

        protected:
            void set_allocated(const size_t bytes) throw();

        private:
            YOCTO_DISABLE_ASSIGN(field_data);
            virtual const void * __get(const coord1d offset_value) const throw() = 0;
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
            
            virtual void save(const layout_type &sub, void       *buf ) const throw() = 0;
            virtual void load(const layout_type &sub, const void *buf )       throw() = 0;



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
            inline virtual const void * __get(const coord1d offset_value) const throw()
            {
                assert(offset_value>=0);
                assert(offset_value<coord1d(num_outer));
                return entry + offset_value;
            }
        };

        template <typename T,typename COORD>
        struct field_for;

    }
}

#endif
