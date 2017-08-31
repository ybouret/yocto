#include "yocto/lang/lexical/scanner.hpp"
#include "yocto/exception.hpp"
#include "yocto/code/utils.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Lexical
        {
            Scanner:: ~Scanner() throw()
            {
            }

#define Y_SCANNER_CTOR()  \
stamp( new string(id) ),   \
label( *stamp),             \
rules(),                     \
module(0),                    \
max_label_length(0),           \
sharedDict(0),                  \
translator(0)

            Scanner:: Scanner(const string &id) :
            Y_SCANNER_CTOR()
            {
            }

            Scanner:: Scanner(const char *id) :
            Y_SCANNER_CTOR()
            {
            }


            const string & Scanner:: key() const throw()
            {
                return label;
            }

            void Scanner:: checkRuleName(const string &rule_label) const
            {
                for(const Rule *r = rules.head;r;r=r->next)
                {
                    if(rule_label==r->label) throw exception("<%s>: multiple rule '%s'", label.c_str(), rule_label.c_str() );
                }
                (size_t &)max_label_length = max_of(max_label_length,rule_label.length());
            }

            Unit * Scanner:: newUnit() const
            {
                return new Unit(stamp);
            }

            Unit * Scanner:: newUnit( Token &content ) const
            {
                Unit *u = new Unit(stamp);
                u->swap_with(content);
                return u;
            }

            const Rule * Scanner:: head() const throw()
            {
                return rules.head;
            }

        }
    }
}


