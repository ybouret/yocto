#include "yocto/lang/lexical/scanner.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Lexical
        {
            Scanner:: ~Scanner() throw()
            {
            }

            Scanner:: Scanner(const string &id) :
            name(id),
            rules()
            {
            }

            const string & Scanner:: key() const throw()
            {
                return name;
            }

        }
    }
}
