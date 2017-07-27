#ifndef YOCTO_LANG_PATTERN_SIMPLE_INCLUDED
#define YOCTO_LANG_PATTERN_SIMPLE_INCLUDED 1

#include "yocto/lang/pattern.hpp"

namespace yocto
{

    namespace Lang
    {

        class Any1 : public Pattern
        {
        public:
            YOCTO_LANG_PATTERN_DECL(Any1,'A', 'N', 'Y', '1');

            explicit Any1() throw();


        private:
            Any1(const Any1 &) throw();
        };

    }
}

namespace yocto
{

    namespace Lang
    {

        class Single : public Pattern
        {
        public:
            YOCTO_LANG_PATTERN_DECL(Single,'S', 'N', 'G', 'L');

            explicit Single(const uint8_t C) throw();

            const uint8_t code;
            
        private:
            Single(const Single &) throw();
        };
        
    }
}

namespace yocto
{

    namespace Lang
    {

        class Range : public Pattern
        {
        public:
            YOCTO_LANG_PATTERN_DECL(Range,':', '-', '>', ':');

            explicit Range(const uint8_t lo, const uint8_t up) throw();

            const uint8_t lower,upper;

        private:
            Range(const Range &) throw();
        };
        
    }
}




#endif


