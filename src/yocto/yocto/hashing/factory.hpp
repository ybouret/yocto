#ifndef YOCTO_HASHING_FACTORY_INCLUDED
#define YOCTO_HASHING_FACTORY_INCLUDED 1

#include "yocto/associative/map.hpp"
#include "yocto/string.hpp"

namespace yocto  {

    namespace hashing
    {
        typedef map<string,function::pointer> hfdb;

        class factory : public hfdb
        {
        public:
            explicit factory() throw();
            explicit factory(const size_t n);
            virtual ~factory() throw();

            void append( function *f );
            function & operator[](const string &name) const;
            function & operator[](const char   *name) const;

            const size_t max_name_length;
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(factory);
        };
    }
    
}

#endif

