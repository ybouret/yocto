#include "yocto/lang/lexical/plugin/cstring.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Lexical
        {

            cstring:: ~cstring() throw()
            {
            }

            cstring:: cstring(const char *id, Translator &trans) :
            yocto::Lang::Lexical::Plugin(id,trans)
            {
                back( trigger(),  this, & cstring::quit  );
                make("CHAR", ".", this, & cstring::grow  );
            }

            const char * cstring::trigger() const throw()
            {
                return "\"";
            }

            void cstring:: startUp(const Token &)
            {
                data.clear();
            }

            Result cstring:: grow(const Token &tkn) throw()
            {
                data.add(tkn);
                return Discard;
            }
        }

    }

}
