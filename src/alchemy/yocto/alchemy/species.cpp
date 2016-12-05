#include "yocto/alchemy/species.hpp"

namespace yocto
{
    namespace alchemy
    {
        species:: ~species() throw()
        {
        }

        species:: species(const string &id, const int zz, const size_t ii) :
        name(id),
        z(zz),
        zsq(z*z),
        indx(ii),
        data()
        {

        }

        const string & species:: key() const throw()
        {
            return name;
        }

    }
}
