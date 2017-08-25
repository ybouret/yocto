#include "yocto/lang/syntax/rule.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            Rule:: ~Rule() throw()
            {
            }

            Rule:: Rule(const string  &id,
                        const uint32_t t) :
            next(0),
            prev(0),
            uuid(t),
            label(id)
            {
            }

            const string & Rule:: key() const throw()
            {
                return label;
            }


        }
    }
}

