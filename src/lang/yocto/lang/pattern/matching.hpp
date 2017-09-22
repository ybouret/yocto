#ifndef YOCTO_LANG_PATTERN_MATCHING_INCLUDED
#define YOCTO_LANG_PATTERN_MATCHING_INCLUDED 1

#include "yocto/lang/pattern.hpp"

namespace yocto
{
    namespace Lang
    {

        class Matching
        {
        public:
            Matching(Pattern *p) throw();
            Matching(const string &expr);
            Matching(const char   *expr);
            Matching(const Matching &);
            virtual ~Matching() throw();

            void assign(Pattern *p) throw();

            bool completely(const string &line) const;
            bool partially(const string &line) const;


        private:
            Pattern::Handle motif;
            YOCTO_DISABLE_ASSIGN(Matching);
            void check();
        };
    }
}

#endif
