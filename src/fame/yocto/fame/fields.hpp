#ifndef YOCTO_FAME_FIELDS_INCLUDED
#define YOCTO_FAME_FIELDS_INCLUDED 1

#include "yocto/fame/field3d.hpp"
#include "yocto/sequence/list.hpp"

namespace yocto
{
    namespace fame
    {
        
#if 0
        typedef vector<field_data::pointer,memory::global::allocator> io_fields;
        
        class fields : public io_fields
        {
        public:
            explicit fields() throw();
            virtual ~fields() throw();
            explicit fields(const size_t n);
            
            size_t block_size() const throw();
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(fields);
        };
#endif
        
        class fields
        {
        public:
            explicit fields() throw();
            virtual ~fields() throw();
            explicit fields(const size_t n);
            
            const size_t block_size;
            
            size_t size() const throw();
            
            void add( const field_data::pointer &p);
            
            
        private:
            list<field_data::pointer> fd;
            YOCTO_DISABLE_COPY_AND_ASSIGN(fields);
        };
        
        
    }
}

#endif
