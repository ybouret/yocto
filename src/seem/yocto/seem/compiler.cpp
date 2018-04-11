#include "yocto/seem/compiler.hpp"
#include "yocto/exception.hpp"

namespace yocto
{
    namespace Seem
    {

        static const char seemCode[] =
        {
#include "seem.inc"
        };

        Compiler:: ~Compiler() throw()
        {
            assert(parser);
            delete parser;
            parser=0;
        }


        Compiler:: Compiler(const bool verbose) :
        parser( Lang::Syntax::Parser::CompileData("Seem Grammar", seemCode, sizeof(seemCode), verbose) )
        {
        }


        vCode Compiler:: compile(Lang::Source &source)
        {
            parser->reset();
            Lang::Syntax::Node *tree = parser->parse(source);
            if(!tree)
            {
                throw exception("SEEM.Compiler: unexpected emptry tree from '%s'!", **source.stamp() );
            }
            return vCode(tree);
        }
    }
}
