#ifndef YOCTO_SEEM_COMPILER_INCLUDED
#define YOCTO_SEEM_COMPILER_INCLUDED 1

#include "yocto/lang/syntax/parser.hpp"
#include "yocto/ptr/arc.hpp"

namespace yocto
{
    namespace Seem
    {
        typedef arc_ptr<Lang::Syntax::Node> vCode;
        
        class Compiler
        {
        public:
            explicit Compiler(const bool verbose=false);
            virtual ~Compiler() throw();

            vCode compile( Lang::Source &source );

            inline const Lang::Syntax::Grammar & gram() { return *parser; }
        private:
            Lang::Syntax::Parser *parser;
            YOCTO_DISABLE_COPY_AND_ASSIGN(Compiler);
        };
    }
}

#endif

