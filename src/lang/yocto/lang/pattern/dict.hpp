#ifndef YOCTO_LANG_PATTERN_DICT_INCLUDED
#define YOCTO_LANG_PATTERN_DICT_INCLUDED 1

#include "yocto/lang/pattern.hpp"
#include "yocto/associative/map.hpp"
#include "yocto/ptr/shared.hpp"

namespace yocto
{
    namespace Lang
    {
        typedef shared_ptr<Pattern>     PatternPtr;
        typedef map<string,PatternPtr>  PatternMap;

        class PatternDict : public PatternMap
        {
        public:
            explicit PatternDict() throw();
            virtual ~PatternDict() throw();

            void define( const string &id, Pattern *p );
            void define( const char   *id, Pattern *p );

            Pattern * operator[](const string &) const;
            Pattern * operator[](const char   *) const;

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(PatternDict);
        };

    }
}

#endif
