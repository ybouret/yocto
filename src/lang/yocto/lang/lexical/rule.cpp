#include "yocto/lang/lexical/rule.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Lexical
        {
            Rule:: ~Rule() throw()
            {
                assert(motif);
                delete motif;
            }

            Rule:: Rule( const Pattern *p, const Action &a ) :
            next(0),
            prev(0),
            motif(p),
            action(a)
            {

            }

            Rule * Rule:: Create( const Pattern *p, const Action &a)
            {
                auto_ptr<const Pattern> guard( p );
                Rule *r = new Rule(p,a);
                guard.forget();
                return r;
            }
        }
        
    }
}
