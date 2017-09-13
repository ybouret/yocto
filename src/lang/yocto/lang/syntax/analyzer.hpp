#ifndef YOCTO_LANG_SYNTAX_ANALYZER_INCLUDED
#define YOCTO_LANG_SYNTAX_ANALYZER_INCLUDED 1

#include "yocto/lang/syntax/grammar.hpp"
#include "yocto/hashing/mph.hpp"
#include "yocto/ios/ostream.hpp"

#define YOCTO_LANG_SYNTAX_ANALYZER_OUTPUT 0

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            class Analyzer
            {
            public:
                static const int RulesShift = 0x03E8;
                virtual ~Analyzer() throw();
                explicit Analyzer(const Grammar &G);

                void walk(const Node *tree, ios::ostream *pfp);

            private:
                hashing::mperf termHash; //!< hashing of terminals
                hashing::mperf ruleHash; //!< hashing of internals
                
                int            depth;    //!< current depth
                size_t         max_size;
                void __walk(const Node *node, ios::ostream *pfp);

                YOCTO_DISABLE_COPY_AND_ASSIGN(Analyzer);
                void __indent(ios::ostream &fp) const;
                void __write(const string &label, ios::ostream &fp) const;
                void __align(const string &label, ios::ostream &fp) const;
            };
        }
    }
}

#endif
