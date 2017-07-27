#ifndef YOCTO_LANG_PATTERN_JOKER_INCLUDED
#define YOCTO_LANG_PATTERN_JOKER_INCLUDED 1

#include "yocto/lang/pattern.hpp"

namespace yocto
{
    namespace Lang
    {
        class Joker : public Pattern
        {
        public:
            virtual ~Joker() throw();

        protected:
            const Handle motif;
            Joker(const uint32_t t,const Handle &p) throw();
            Joker(const Joker &) throw();

        private:
            YOCTO_DISABLE_ASSIGN(Joker);
        };
    }
}

namespace yocto
{
    namespace Lang
    {
        class Optional : public Joker
        {
        public:
            YOCTO_LANG_PATTERN_DECL(Optional, ' ', '|', '|', ' ');
            explicit Optional(const Handle &p) throw();

        private:
            Optional(const Optional &) throw();
        };
    }
}

#endif
