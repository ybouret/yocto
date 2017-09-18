#ifndef YOCTO_LANG_SYNTAX_ANALYZER_INCLUDED
#define YOCTO_LANG_SYNTAX_ANALYZER_INCLUDED 1

#include "yocto/lang/syntax/grammar.hpp"
#include "yocto/hashing/mph.hpp"
#include "yocto/ios/ostream.hpp"
#include "yocto/sequence/list.hpp"

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
                explicit Analyzer(const Grammar &);

                void walk(const Node *tree);

                virtual void onTerminal(const string &label,
                                        const int     hCode,
                                        const string &content);

                virtual void onInternal(const string &label,
                                        const int     hCode,
                                        const int     nArgs);

                void emitDefinitions( ios::ostream &fp, const string &prefix ) const;


            private:
                Hasher       terminalHash; //!< hashing of terminals
                Hasher       internalHash; //!< hashing of internals
                list<string> terminals;
                list<string> internals;
                void  __walk(const Node *node);
                void  __updateMaxSize(const size_t n) const throw();
                YOCTO_DISABLE_COPY_AND_ASSIGN(Analyzer);

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
