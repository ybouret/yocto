#ifndef YOCTO_LANG_SYNTAX_COMPILER_INCLUDED
#define YOCTO_LANG_SYNTAX_COMPILER_INCLUDED 1

#include "yocto/lang/syntax/parser.hpp"
#include "yocto/lang/syntax/analyzer.hpp"

namespace yocto
{
    namespace Lang
    {
        //! smart pointer for a generated parser
        class DynamicParser
        {
        public:
            explicit DynamicParser(Syntax::Parser *p) throw();
            virtual ~DynamicParser() throw();
            
        protected:
            auto_ptr<Syntax::Parser> impl;

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(DynamicParser);
        };

        //! a language compiler: load the AST and walk it
        class Compiler :
        public DynamicParser,
        public Syntax::Analyzer
        {
        public:
            virtual ~Compiler() throw();
            explicit Compiler(Syntax::Parser *p);

            virtual void initialize();         //!< default: do nothing, but should clear stacks....
            void         ld(Source &source);   //!< load source and walk the AST


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Compiler);
        };
    }
}

#endif
