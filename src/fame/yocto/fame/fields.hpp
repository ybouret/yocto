#ifndef YOCTO_FAME_FIELDS_INCLUDED
#define YOCTO_FAME_FIELDS_INCLUDED 1

#include "yocto/fame/field3d.hpp"
#include "yocto/sequence/list.hpp"

namespace yocto
{
    namespace fame
    {
        
        class fields
        {
        public:
            typedef list<field_data::pointer> fd_list;
            typedef fd_list::iterator         iterator;
            typedef fd_list::const_iterator   const_iterator;
            
            explicit fields() throw();
            virtual ~fields() throw();
            explicit fields(const size_t n);
            
            const size_t block_size;
            
            void   add( const field_data::pointer &p);
            
            inline size_t         size()  const throw() { return fd.size(); }
            inline iterator       begin() throw() { return fd.begin(); }
            inline iterator       end()   throw() { return fd.end();   }
            inline const_iterator begin() const throw() { return fd.begin(); }
            inline const_iterator end()   const throw() { return fd.end();   }
            
            
        private:
            fd_list fd;
            YOCTO_DISABLE_COPY_AND_ASSIGN(fields);
        };
        
        
    }
}

#endif
