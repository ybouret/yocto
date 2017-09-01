#ifndef YOCTO_LANG_SYNTAX_COMPOUND_INCLUDED
#define YOCTO_LANG_SYNTAX_COMPOUND_INCLUDED 1

#include "yocto/lang/syntax/rule.hpp"
#include "yocto/sequence/addr-list.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {


            typedef addr_node<Rule> MetaNode;
            typedef addr_list<Rule> MetaList;

            //! a container for aggregate/alternate
            class Compound : public Rule
            {
            public:
                virtual ~Compound() throw();

                MetaList members;

                Compound & operator<<( Rule & );

            protected:
                explicit Compound(const string &, const uint32_t);
                void     vizLink( ios::ostream &fp ) const;


            private:
                YOCTO_DISABLE_COPY_AND_ASSIGN(Compound);
            };

        }
    }
}

#endif
