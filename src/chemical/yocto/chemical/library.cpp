
#include "yocto/chemical/library.hpp"
#include "yocto/exception.hpp"

namespace yocto
{
    namespace chemical
    {
        library:: ~library() throw() {}

        library:: library( const string &id, const size_t reserved ) :
        species_set(reserved,as_capacity),
        name(id)
        {
        }


        species & library:: add( const string &sp_name, const int sp_z )
        {
            compile();
            species::pointer p( new species(sp_name,sp_z) );
            if( ! insert(p) )
            {
                throw exception("library: multiple '%s'", *sp_name);
            }
            (size_t &)(p->indx) = size();
            return *p;
        }

        void library:: compile() throw()
        {
            size_t j=0;
            for(iterator i=begin();i!=end();++i)
            {
                species &s = **i;
                (size_t&)(s.indx) = ++j;
            }
        }

        

    }
}

