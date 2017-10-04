#ifndef YOCTO_PATTERN_REGEXP_INCLUDED
#define YOCTO_PATTERN_REGEXP_INCLUDED 1

#include "yocto/lang/pattern/dict.hpp"

namespace yocto
{
    namespace Lang
    {

        Pattern *RegExp( const string &expr, const PatternDict *dict );
        Pattern *RegExp( const char   *expr, const PatternDict *dict );
    }
}

#endif
