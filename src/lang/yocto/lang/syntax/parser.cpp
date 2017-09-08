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
            Lexer(id+".lexer"),
            tag(id)
            {
            }

            Terminal & Parser:: terminal( const string &label, const string &expr)
            {
                root.make(label,expr,YOCTO_LANG_LEXICAL(forward));
                return add( new Terminal(label));
            }

            Terminal & Parser:: terminal(const string &label, const uint8_t code)
            {
                root.make(label,new Single(code),YOCTO_LANG_LEXICAL(forward));
                return add( new Terminal(label) );
            }


            Node * Parser:: parse(Source &source)
            {
                Lexer &lexer = *this;
                lexer.reset();
                return Node::AST(accept(lexer,source));
            }

            void Parser:: check() const
            {
                check_connectivity();
                for(const Lexical::Rule *r = root.head();r;r=r->next)
                {

                }
            }

            Terminal & Parser:: plugin(const string &label)
            {
                root.call(label);
                return add( new Terminal(label) );
            }
            
        }
        
    }
    
}
