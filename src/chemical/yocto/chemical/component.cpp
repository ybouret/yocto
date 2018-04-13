
#include "yocto/chemical/component.hpp"
#include "yocto/exception.hpp"

namespace yocto
{
    namespace chemical
    {
        component:: ~component() throw() {}

        component:: component(species &s, const int coef) :
        sp( &s ),
        nu( coef )
        {
            assert(sp->refcount()>1);
            if(!nu) throw exception("component: no stoichiometry");
        }

        const string & component:: key() const throw()
        {
            return sp->name;
        }

    }

}

