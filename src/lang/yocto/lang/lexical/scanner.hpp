#ifndef YOCTO_LANG_LEXICAL_SCANNER_INCLUDED
#define YOCTO_LANG_LEXICAL_SCANNER_INCLUDED 1

#include "yocto/lang/lexical/rule.hpp"
#include "yocto/lang/source.hpp"
#include "yocto/ptr/intr.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Lexical
        {
            class Scanner : public counted_object
            {
            public:
                typedef intr_ptr<string,Scanner> Handle;

                const string name;

                explicit Scanner(const string &id);
                virtual ~Scanner() throw();

                const string & key() const throw();
                

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Scanner);
                Rules rules;
            };
        }
    }
}

#endif
