#include "yocto/chemical/species.hpp"

namespace yocto
{
    namespace chemical
    {
        species:: ~species() throw()
        {
        }

        species:: species( const string &id, const int charge ) :
        name(id),
        z(charge),
        indx(0)
        {
        }


        const string & species:: key() const throw()
        {
            return name;
        }

        
    }

}

