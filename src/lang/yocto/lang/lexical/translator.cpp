#include "yocto/lang/lexical/translator.hpp"

#include <iostream>

namespace yocto
{
    namespace Lang
    {
        namespace Lexical
        {
            Translator:: ~Translator() throw() {}

#define Y_TRANS_CTOR() \
name(transID),          \
scanner(0),              \
scanners(1,as_capacity)

            Translator:: Translator(const string &transID,
                                    const string &scanrID) :
            Y_TRANS_CTOR()
            {
                initialize(scanrID);
            }

            Translator:: Translator(const char *transID,
                                    const char *scanrID) :
            Y_TRANS_CTOR()
            {
                const string __sid(scanrID);
                initialize(__sid);
            }


            void Translator:: initialize(const string &scanrID)
            {
                std::cerr << "db.capacity=" << scanners.capacity() << std::endl;
                const Scanner::Handle s( new Scanner(scanrID) );
                
            }

        }
    }
}
