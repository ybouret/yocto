#include "yocto/utest/run.hpp"
#include "yocto/seem/compiler.hpp"

#include "yocto/ios/icstream.hpp"
#include "yocto/ios/ocstream.hpp"

#include "yocto/ios/graphviz.hpp"
#include "yocto/lang/syntax/analyzer.hpp"

using namespace yocto;

YOCTO_UNIT_TEST_IMPL(compiler)
{

    Seem::Compiler SEEMC( true );
    Lang::Source   source( Lang::Module::OpenSTDIN());

    Seem::vCode tree = SEEMC.compile(source);
    tree->graphviz(       SEEMC.gram().name + "_output.dot" );
    ios::graphviz_render( SEEMC.gram().name + "_output.dot" );

    Lang::Syntax::Analyzer analyzer( SEEMC.gram() );
    analyzer.walk( tree.__get() );
    
}
YOCTO_UNIT_TEST_DONE()
