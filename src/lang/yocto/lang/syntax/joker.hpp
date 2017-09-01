#ifndef YOCTO_LANG_SYNTAX_JOKER_INCLUDED
#define YOCTO_LANG_SYNTAX_JOKER_INCLUDED 1

#include "yocto/lang/syntax/rule.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            //! base class for a joker rule
            class Joker : public Rule
            {
            public:
                virtual ~Joker() throw();

                const Rule *content() const throw();

            protected:
                explicit Joker(const string &, const uint32_t, Rule &);
                Rule    &rule;

            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Joker);
            };

        }

    }
}


#endif
