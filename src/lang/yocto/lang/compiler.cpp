#include "yocto/lang/compiler.hpp"
#include "yocto/exception.hpp"


namespace yocto
{
    namespace Lang
    {
        DynamicParser:: DynamicParser( Syntax::Parser *p ) throw() :
        impl(p)
        {
            assert(p!=NULL);
        }

        DynamicParser:: ~DynamicParser() throw()
        {
        }

    }
}

namespace yocto
{
    namespace Lang
    {

        Compiler:: ~Compiler() throw()
        {
        }

        Compiler:: Compiler( Syntax::Parser *p ) :
        DynamicParser(p),
        Syntax::Analyzer(*impl)
        {
        }

        void Compiler:: ld(Source &source)
        {
            auto_ptr<Syntax::Node> tree( impl->parse(source) );
            if(!tree.is_valid())
            {
                throw exception("%s: empty AST", impl->tag.c_str() );
            }
            walk( & *tree );
        }

    }
}
