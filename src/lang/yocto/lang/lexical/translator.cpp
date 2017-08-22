#include "yocto/lang/lexical/translator.hpp"
#include "yocto/exception.hpp"

#include <iostream>

namespace yocto
{
    namespace Lang
    {
        namespace Lexical
        {
            Translator:: ~Translator() throw() {}

#define Y_TRANS_CTOR()        \
name(transID),                 \
__root( new Scanner(scanrID) ), \
root( *__root ),                 \
current( &root ),                 \
scanners(1,as_capacity)

#define Y_TRANS_INIT() if( ! scanners.insert( __root ) ) \
throw exception("[%s]: unable to initialize scanner <%s>", name.c_str(), root.name.c_str() )

            Translator:: Translator(const string &transID,
                                    const string &scanrID) :
            Y_TRANS_CTOR()
            {
                Y_TRANS_INIT();
            }

            Translator:: Translator(const char *transID,
                                    const char *scanrID) :
            Y_TRANS_CTOR()
            {
                Y_TRANS_INIT();
            }

            Scanner & Translator:: define(const string &scanrID)
            {
                Scanner::Handle s( new Scanner(scanrID) );
                if(!scanners.insert(s))
                {
                    throw exception("[%s]: multiple scanner <%s>", name.c_str(),scanrID.c_str());
                }
                return *s;
            }

            Scanner & Translator:: define(const char *scanrID)
            {
                const string sid(scanrID);
                return define(sid);
            }

        }
    }
}
