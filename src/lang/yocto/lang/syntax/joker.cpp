#include "yocto/lang/syntax/joker.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {
            Joker:: ~Joker() throw()
            {
            }

            Joker:: Joker(const string &id,
                          const uint32_t t,
                          Rule          &r) :
            Rule(id,t),
            rule(r)
            {
            }

            const Rule * Joker:: content() const throw()
            {
                return &rule;
            }

        }
    }
}
