#include "yocto/lang/pattern/basic.hpp"
#include "yocto/lang/pattern/logic.hpp"
#include "yocto/lang/pattern/joker.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ptr/auto.hpp"
#include "yocto/ios/graphviz.hpp"

using namespace yocto;
using namespace Lang;

YOCTO_UNIT_TEST_IMPL(pattern)
{
    {
        Source         source( Module::OpenSTDIN() );
        source.prefetch(10);

        {
            Patterns patterns;
            patterns.push_back( new Any1()      );
            patterns.push_back( new Single('A') );
            patterns.push_back( new AND() );
            patterns.push_back( new OR() );
            patterns.push_back( new Range('a','z') );
            patterns.push_back( Optional::Create( new Single('c') ) );
            patterns.push_back( ZeroOrMore( new AND() ) );
            patterns.push_back( OneOrMore( new Any1() ) );
            patterns.push_back( new NONE() );
            Patterns patterns2(patterns);
        }


        auto_ptr<Pattern> motif( Logical::Equal("hello") );

        {
            motif->graphviz("pat.dot");
            ios::graphviz_render("pat.dot");
        }



        while( source.is_active() )
        {
            assert(source.in_cache()>=1);
            Token tk;
            if(motif->match(source,tk))
            {
                std::cerr << "<" << tk << ">";
                continue;
            }
            assert(0==tk.size);

            assert(source.in_cache()>=1);
            std::cerr << char(source.peek()->code);
            source.forward(1);
        }

    }
}
YOCTO_UNIT_TEST_DONE()

