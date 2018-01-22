
#ifndef YOCTO_IPSO_FIELD_INCLUDED
#define YOCTO_IPSO_FIELD_INCLUDED 1

#include "yocto/ipso/patch.hpp"
#include "yocto/counted-object.hpp"
#include "yocto/ptr/intr.hpp"


#include <cstring>

namespace yocto
{
    namespace ipso
    {

        class field_info : public counted_object
        {
        public:
            typedef intr_ptr<string,field_info> pointer;

            const string name;
            const size_t count; //!< items count, set by field after setup
            const size_t bytes; //!< bytes count, set by field after setup
            
            virtual ~field_info() throw();
            const string &key() const throw();
            
        protected:
            explicit field_info(const char   *id);

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(field_info);
        };

        template <typename T>
        class field : public field_info
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;

            inline virtual ~field() throw() {}

            T           *entry; //!< linear items


            inline void ldz() throw() { assert(entry); assert(bytes); memset(entry,0,bytes); }
            inline void ld( param_type arg ) throw()
            {
                assert(entry); assert(bytes); 
                for(size_t i=0;i<count;++i) memcpy(&entry[i],&arg,sizeof(T));
            }

        protected:
            inline explicit field(const char *id) :
            field_info(id), entry(0) {}
            
            void set_bytes(const size_t items) throw()
            {
                (size_t &)count = items;
                (size_t &)bytes = items*sizeof(T);
            }

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(field);
        };

    }
}

#endif


