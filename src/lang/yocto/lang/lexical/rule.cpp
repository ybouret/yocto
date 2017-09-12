#include "yocto/lang/lexical/rule.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Lexical
        {
            Rule:: ~Rule() throw()
            {
            }

            Rule:: Rule(const string          &l,
                        const Pattern::Handle &p,
                        const Action          &a) :
            next(0),
            prev(0),
            stamp( new string(l) ),
            label( *stamp ),
            motif(p),
            action(a)
            {
                
            }


        }
        
    }
}
