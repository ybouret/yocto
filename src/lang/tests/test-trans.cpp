#include "yocto/lang/pattern/regexp.hpp"
#include "yocto/lang/lexical/translator.hpp"
#include "yocto/utest/run.hpp"


using namespace yocto;
using namespace Lang;

class myTrans : public Lexical::Translator
{
public:

    explicit myTrans() : Lexical::Translator("lexer","main")
    {
    }

    virtual ~myTrans() throw()
    {
    }


private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(myTrans);
};


YOCTO_UNIT_TEST_IMPL(trans)
{
    myTrans trans;

}
YOCTO_UNIT_TEST_DONE()
