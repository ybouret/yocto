#include "yocto/lang/char.hpp"
#include "yocto/lang/syntax/rules.hpp"

#include "yocto/utest/run.hpp"
using namespace yocto;
using namespace Lang;

static inline
void __display(const char *text)
{
    assert(text);
    static const size_t width = 32;
    const size_t        length = length_of(text);
    if(length>=width)
    {
        std::cerr << text;
    }
    else
    {
        const size_t delta = width-length;
        const size_t pre   = delta/2;
        const size_t post  = delta-pre;
        for(size_t i=0;i<pre;++i) std::cerr << ' ';
        std::cerr << text;
        for(size_t i=0;i<post;++i) std::cerr << ' ';
    }
}


#define __SHOW(TYPE) do { \
std::cerr << "sizeof( ";  \
__display(#TYPE);\
std::cerr << " ) = " << sizeof(TYPE) << std::endl;\
} while(false)

YOCTO_UNIT_TEST_IMPL(types)
{
    __SHOW(CharInfo);
    __SHOW(Char);
    __SHOW(Syntax::Node);
    __SHOW(Syntax::Node::List);
    __SHOW(Syntax::Rule);
    __SHOW(Syntax::Terminal);
    __SHOW(Syntax::Alternate);
    __SHOW(Syntax::Aggregate);
    __SHOW(Syntax::Optional);
    __SHOW(Syntax::Counting);

}
YOCTO_UNIT_TEST_DONE()
