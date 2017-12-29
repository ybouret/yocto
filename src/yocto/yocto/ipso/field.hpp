
#ifndef YOCTO_IPSO_FIELD_INCLUDED
#define YOCTO_IPSO_FIELD_INCLUDED 1

#include "yocto/ipso/patch.hpp"
#include "yocto/counted-object.hpp"
#include <cstring>

namespace yocto
{
    namespace ipso
    {



        template <typename T>
        class field : public counted_object
        {
        public:
            YOCTO_ARGUMENTS_DECL_T;

            inline virtual ~field() throw() {}
            T           *entry; //!< linear items
            const size_t count; //!< items count
            const size_t bytes; //!< bytes
            inline void ldz() throw() { memset(entry,0,bytes); }
            inline void ld( param_type arg ) throw()
            {
                for(size_t i=0;i<count;++i) memcpy(&entry[i],&arg,sizeof(T));
            }

        protected:
            inline explicit field() throw() :
            entry(0),
            count(0),
            bytes(0)
            {
            }

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


