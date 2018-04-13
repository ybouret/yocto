#ifndef YOCTO_CHEMICAL_LIBRARY_INCLUDED
#define YOCTO_CHEMICAL_LIBRARY_INCLUDED 1

#include "yocto/chemical/species.hpp"
#include "yocto/associative/set.hpp"
#include "yocto/sequence/array.hpp"

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

            species & operator()(const string &);
            species & operator()(const char   *);


            void compile() throw();
            const size_t max_name_length;
            friend std::ostream & operator<<( std::ostream &os, const library &lib);

            void display( const array<double> &C ) const;


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(library);
        };

    }
}

#endif

