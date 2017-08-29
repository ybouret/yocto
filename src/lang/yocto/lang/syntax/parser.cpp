#include "yocto/lang/syntax/parser.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {
            Parser:: ~Parser() throw()
            {
            }

            Parser:: Parser( const string &id ) :
            Grammar(id+".grammar"),
            Lexer(id+".lexer")
            {
            }

            Rule & Parser:: terminal( const string &label, const string &expr )
            {
                root.make(label,expr,YOCTO_LANG_LEXICAL(forward));
                return add( new Terminal(label) );
            }


            Node * Parser:: parse(Source &source)
            {
                Lexer &lexer = *this;
                lexer.reset();
                return accept(lexer,source);
            }
        }

    }

}
