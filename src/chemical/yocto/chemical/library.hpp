#ifndef YOCTO_CHEMICAL_LIBRARY_INCLUDED
#define YOCTO_CHEMICAL_LIBRARY_INCLUDED 1

#include "yocto/chemical/species.hpp"
#include "yocto/associative/set.hpp"

namespace yocto
{
    namespace chemical
    {
        typedef set<string,species::pointer> species_set;

        class  library :
        public species_set,
        public counted
        {
        public:
            typedef intr_ptr<string,library> pointer;

            const string name;

            explicit library(const string &id,const size_t reserved=16);
            virtual ~library() throw();

            species & add( const string &sp_name, const int sp_z );

            void compile() throw();
            
        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(library);
        };

    }
}

#endif

