#ifndef YOCTO_IPSO_FIELDS_INCLUDED
#define YOCTO_IPSO_FIELDS_INCLUDED 1

#include "yocto/ipso/field3d.hpp"

namespace yocto
{
    namespace ipso
    {

        typedef vector<field_info::pointer> fields_type;

        
        class fields : public fields_type
        {
        public:
            explicit fields() throw();
            explicit fields(const size_t n);
            virtual ~fields() throw();

            void append( field_info &shared );

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(fields);
        };
    }
}

#endif

