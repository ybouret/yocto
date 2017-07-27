#ifndef YOCTO_LANG_PATTERN_LOGIC_INCLUDED
#define YOCTO_LANG_PATTERN_LOGIC_INCLUDED 1

#include "yocto/lang/pattern.hpp"

namespace yocto
{
    namespace Lang
    {
        class Logical : public Pattern
        {
        public:
            virtual ~Logical() throw();
            Patterns operands;

        protected:
            explicit Logical(const uint32_t t) throw();
            Logical(const Logical &other);

        private:
            YOCTO_DISABLE_ASSIGN(Logical);
        };
    }
}

namespace yocto
{
    namespace Lang
    {
        class AND : public Logical
        {
        public:
            YOCTO_LANG_PATTERN_DECL(AND, ' ', '&', '&', ' ');
            explicit AND() throw();

        private:
            AND(const AND &);
        };
    }
}



#endif
