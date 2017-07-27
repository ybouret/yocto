#include "yocto/lang/pattern/basic.hpp"
#include "yocto/lang/pattern/logic.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ptr/auto.hpp"

using namespace yocto;

using namespace Lang;

YOCTO_UNIT_TEST_IMPL(pattern)
{
    {
        Module::Handle hModule( new Module() );
        Source         source( hModule );
        source.prefetch(10);

        {
            Patterns patterns;
            patterns.push_back( new Any1()      );
            patterns.push_back( new Single('A') );
            patterns.push_back( new AND() );
            patterns.push_back( new OR() );

            Patterns patterns2(patterns);
        }

        auto_ptr<Pattern> motif( Logical::Equal("hello") );
        
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

