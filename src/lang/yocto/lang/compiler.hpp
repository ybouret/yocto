#ifndef YOCTO_LANG_SYNTAX_COMPILER_INCLUDED
#define YOCTO_LANG_SYNTAX_COMPILER_INCLUDED 1

#include "yocto/lang/syntax/parser.hpp"
#include "yocto/lang/syntax/analyzer.hpp"

namespace yocto
{
    namespace Lang
    {
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

        class Compiler :
        public DynamicParser,
        public Syntax::Analyzer
        {
        public:
            virtual ~Compiler() throw();
            explicit Compiler(Syntax::Parser *p);

            virtual void initialize();
            void         ld(Source &source);


        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Compiler);
        };
    }
}

#endif
