#ifndef YOCTO_LANG_LEXICAL_UNIT_INCLUDED
#define YOCTO_LANG_LEXICAL_UNIT_INCLUDED 1

#include "yocto/lang/source.hpp"

namespace yocto
{
    namespace Lang
    {

        namespace Lexical
        {

            class Unit; //!< forward declaration for Units
            typedef core::list_of_cpp<Unit> Units;

            class Unit : public Token
            {
            public:
                virtual ~Unit() throw();
                explicit Unit(const Stamp &ruleStamp) throw();
                Unit(const Unit &other);

                Unit         *next;
                Unit         *prev;
                const Stamp   stamp;
                const string &label;
                inline const char   *id() const throw() { return label.c_str(); }

                static void RemoveFrom( Units &units, const string &id) throw();

            private:
                YOCTO_DISABLE_ASSIGN(Unit);
            };



        }
    }
}

#endif
