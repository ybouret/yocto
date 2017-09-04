#include "yocto/lang/gen/gcompiler.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            const char gCompiler::name[] = "Grammar Compiler";

            gCompiler:: ~gCompiler() throw()
            {
            }

            gCompiler:: gCompiler() :
            getAST(),
            verbose(false)
            {

            }
            
        }
    }
}

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            Parser * gCompiler:: encode(const Node *ast)
            {
                assert(ast);
                assert(ast->internal);
                assert(ast->toList().size>=1);

                const Node::List &topList    = ast->toList();
                const Node       *topNode    = topList.head;
                const string      parserName = topNode->toLex().toString(1);
                if(verbose) { std::cerr << "Creating parser '" << parserName << "'" << std::endl; }
                auto_ptr<Parser>  p( new Parser(parserName) );

                return p.yield();
            }

        }

    }
}
