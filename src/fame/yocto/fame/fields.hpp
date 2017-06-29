#ifndef YOCTO_FAME_FIELDS_INCLUDED
#define YOCTO_FAME_FIELDS_INCLUDED 1

#include "yocto/fame/field3d.hpp"
#include "yocto/associative/set.hpp"

namespace yocto
{
    namespace fame
    {

        //! database of fields, for multiple I/O
        class fields
        {
        public:
            typedef set<string,field_data::pointer> fd_list;
            typedef fd_list::iterator               iterator;
            typedef fd_list::const_iterator         const_iterator;
            
            explicit fields() throw();
            virtual ~fields() throw();
            explicit fields(const size_t n);
            
            const size_t block_size;
            

            
            inline size_t         size()  const throw() { return fd.size();  }
            inline iterator       begin() throw()       { return fd.begin(); }
            inline iterator       end()   throw()       { return fd.end();   }
            inline const_iterator begin() const throw() { return fd.begin(); }
            inline const_iterator end()   const throw() { return fd.end();   }

            template <typename FIELD>
            inline FIELD & record( FIELD *f )
            {
                const field_data::pointer p(f);
                __record(p);
                return *f;
            }


        private:
            fd_list fd;
            void __record(const field_data::pointer &p);
            YOCTO_DISABLE_COPY_AND_ASSIGN(fields);
        };
        
        
    }
}

#endif
