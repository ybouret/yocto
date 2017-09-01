#ifndef YOCTO_LANG_SYNTAX_COUNTING_INCLUDED
#define YOCTO_LANG_SYNTAX_COUNTING_INCLUDED 1

#include "yocto/lang/syntax/joker.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            //! create a counting rule
            class Counting : public Joker
            {
            public:
                static const uint32_t UUID = YOCTO_FOURCC(' ', '>', '=', ' ');

                virtual ~Counting() throw();

                explicit Counting(const string &,
                                  Rule         &,
                                  const size_t);

                YOCTO_LANG_SYNTAX_RULE_DECL();

                const size_t minCount;

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Counting);
            };

        }

    }
}


#endif
