#include "yocto/lang/lexical/plugin/cstring.hpp"
#include "yocto/lang/lexical/translator.hpp"

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
            Plugin(id,trans),
            data()
            {
                init();
            }

            cstring:: cstring(const string &id, Translator &trans) :
            Plugin(id,trans),
            data()
            {
                init();
            }

            void cstring:: init()
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

            void cstring:: quit(const Token &)
            {
                assert(translator);
                translator->unget( newUnit(data) );
            }
        }

    }

}
