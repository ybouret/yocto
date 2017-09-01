#ifndef YOCTO_LANG_PATTERN_POSIX_INCLUDED
#define YOCTO_LANG_PATTERN_POSIX_INCLUDED 1

#include "yocto/lang/pattern.hpp"

namespace yocto
{
    namespace Lang
    {

        struct posix
        {
            //__________________________________________________________________
            //
            // standard
            //__________________________________________________________________
            static Pattern *upper();  //!< [A-Z]
            static Pattern *lower();  //!< [a-z]
            static Pattern *digit();  //!< [0-9]
            static Pattern *alpha();  //!< [A-Za-z]
            static Pattern *alnum();  //!< [0-9A-Za-z]
            static Pattern *xdigit(); //!< [0-9A-Fa-f]
            static Pattern *blank();  //!< [ \t]
            static Pattern *space();  //!< [ \t\r\n\v\f]
            static Pattern *punct();  //!< !"#$%&'()*+,\-./:;<=>?@[\\\]^_`{|}~

            //__________________________________________________________________
            //
            // not standard
            //__________________________________________________________________
            static Pattern *word();    //!< [A-Za-z0-9_]
            static Pattern *endl();    //!< \n|\r|\r\n
            static Pattern *dot();     //!< [^\n\r]
            static Pattern *any1();    //!< ...
        };
        
    }
}

#endif
