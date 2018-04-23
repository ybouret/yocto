
#include "yocto/chemical/actor.hpp"
#include "yocto/exception.hpp"


namespace yocto
{
    namespace chemical
    {
        actor:: ~actor() throw()
        {
        }
        
        actor:: actor(species &s, const int coef) :
        sp( &s ),
        nu( coef ),
        ev( (nu<0) ? -nu : nu ),
        evm( ev-1 ),
        next(0),
        prev(0)
        {
            assert(sp->refcount()>1);
            if(!nu) throw exception("actor: zero stoichiometry");
        }
        
    }
    
}

