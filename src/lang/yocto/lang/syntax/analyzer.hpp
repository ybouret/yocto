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

                void walk(const Node *tree);

                virtual void onTerminal(const string &label,
                                        const int     hCode,
                                        const string &content);

                virtual void onInternal(const string &label,
                                        const int     hCode,
                                        const size_t  nArgs);

            private:
                Hasher  termHash; //!< hashing of terminals
                Hasher  ruleHash; //!< hashing of internals
                void  __walk(const Node *node);

                YOCTO_DISABLE_COPY_AND_ASSIGN(Analyzer);
                //void __indent(ios::ostream &fp) const;
                //void __write(const string &label, ios::ostream &fp) const;
                //void __align(const string &label, ios::ostream &fp) const;

            public:
                const int      depth;    //!< current depth
                const size_t   max_size;
                std::ostream & __indent() const;
                std::ostream & __output(const string &label) const;
            };
        }
    }
}

#endif
