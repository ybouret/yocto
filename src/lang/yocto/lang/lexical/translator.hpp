#ifndef YOCTO_LANG_LEXICAL_TRANSLATOR_INCLUDED
#define YOCTO_LANG_LEXICAL_TRANSLATOR_INCLUDED 1

#include "yocto/lang/lexical/scanner.hpp"
#include "yocto/associative/set.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Lexical
        {

            class Translator : public object
            {
            public:
                typedef set<string,Scanner::Handle> ScannerDB;
                explicit Translator(const string &transID,
                                    const string &scanrID);
                
                explicit Translator(const char *transID,
                                    const char *scanrID);

                virtual ~Translator() throw();

                const string name;

                Scanner &declare(const string &scanrID);
                Scanner &declare(const char   *scanrID);

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Translator);
                Scanner::Handle __root;
                Units            units;
                
            public:
                Scanner        &root;

            private:
                Scanner        *current;  //!< current scanner
                ScannerDB       scanners; //!< the scanners
                void            onInit();

            public:
                PatternDict     dict;
            };
        }
    }
}

#endif
