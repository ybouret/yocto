#include "yocto/lang/syntax/parser.hpp"
#include "yocto/lang/pattern/basic.hpp"

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

            Rule & Parser:: terminal( const string &label, const string &expr)
            {
                root.make(label,expr,YOCTO_LANG_LEXICAL(forward));
                return add( new Terminal(label));
            }

            Rule & Parser:: terminal(const string &label, const uint8_t code)
            {
                root.make(label,new Single(code),YOCTO_LANG_LEXICAL(forward));
                return add( new Terminal(label) );
            }


            Node * Parser:: operator()(Source &source)
            {
                Lexer &lexer = *this;
                lexer.reset();
                return Node::AST(accept(lexer,source));
            }

            void Parser:: compile() const
            {
                check_connectivity();
                for(const Lexical::Rule *r = root.head();r;r=r->next)
                {
                    
                }
            }

            Rule & Parser:: plugin(const string &label)
            {
                root.call(label);
                return add( new Terminal(label) );
            }

        }

    }

}
