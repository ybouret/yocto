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
units(),                         \
root( *__root ),                  \
current( &root ),                  \
scanners(1,as_capacity)

            
            void Translator:: onInit()
            {
                if( ! scanners.insert( __root ) )
                    throw exception("[%s]: unable to initialize scanner <%s>", name.c_str(), root.name.c_str() );
                __root->sharedDict = &dict;
            }

            Translator:: Translator(const string &transID,
                                    const string &scanrID) :
            Y_TRANS_CTOR()
            {
                onInit();
            }

            Translator:: Translator(const char *transID,
                                    const char *scanrID) :
            Y_TRANS_CTOR()
            {
                onInit();
            }

            Scanner & Translator:: declare(const string &scanrID)
            {
                Scanner::Handle s( new Scanner(scanrID) );
                if(!scanners.insert(s))
                {
                    throw exception("[%s]: multiple scanner <%s>", name.c_str(),scanrID.c_str());
                }
                s->sharedDict = &dict;
                return *s;
            }

            Scanner & Translator:: declare(const char *scanrID)
            {
                const string sid(scanrID);
                return declare(sid);
            }

        }
    }
}
