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
            void optimize() throw();
            
        protected:
            const Pattern *motif;
            Joker(const uint32_t  t,
                  const Pattern  *p) throw();
            Joker(const Joker &);

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
            YOCTO_LANG_PATTERN_DECL(Optional, ' ', ' ', ' ', '?');

            static Pattern *Create(Pattern *p);

        private:
            Optional(const Optional &) ;
            Optional(const Pattern  *) throw();
        };
    }
}

namespace yocto
{
    namespace Lang
    {
        class AtLeast : public Joker
        {
        public:
            YOCTO_LANG_PATTERN_DECL(AtLeast,' ','>','=',' ');
            const size_t count;

            static Pattern * Create( Pattern *p, const size_t n);

        private:
            AtLeast(const Pattern *,const size_t ) throw();
            AtLeast(const AtLeast &);
        };

        inline Pattern *ZeroOrMore( Pattern *p ) { return AtLeast::Create(p,0); }
        inline Pattern *OneOrMore(  Pattern *p ) { return AtLeast::Create(p,1); }
        
    }
}

namespace yocto
{
    namespace Lang
    {
        class Counting : public Joker
        {
        public:
            YOCTO_LANG_PATTERN_DECL(Counting,'<','=','=','>');
            const size_t min_count;
            const size_t max_count;

            static Pattern * Create( Pattern *p, const size_t n);

        private:
            Counting(const Pattern *,const size_t,const size_t) throw();
            Counting(const Counting &);
        };

    }
}



#endif
