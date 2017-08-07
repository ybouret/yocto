#include "yocto/lang/lexical/scanner.hpp"
#include "yocto/exception.hpp"

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

            void Scanner:: checkRuleName(const string &label) const
            {
                for(const Rule *r = rules.head;r;r=r->next)
                {
                    if(label==r->label) throw exception("%s: multiple rule '%s'", name.c_str(), label.c_str() );
                }
            }

        }
    }
}
