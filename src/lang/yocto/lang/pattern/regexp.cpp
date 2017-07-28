#include "yocto/lang/pattern/regexp.hpp"

namespace yocto
{
    namespace Lang
    {
        class RegExpCompiler
        {
        public:
            const char        *curr;
            const char        *last;
            const PatternDict *dict;

            inline RegExpCompiler(const string      &expr,
                                    const PatternDict *dict)
            {
            }

            inline ~RegExpCompiler() throw()
            {
            }


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(RegExpCompiler);
        };
    }
}


namespace yocto
{
    namespace Lang
    {

        Pattern *RegExp( const string &expr, const PatternDict *dict )
        {
            RegExpCompiler compiler(expr,dict);
            
            return NULL;
        }

    }
}
