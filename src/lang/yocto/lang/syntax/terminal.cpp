#include "yocto/lang/syntax/terminal.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            Terminal:: ~Terminal() throw()
            {

            }

            Terminal:: Terminal(const string &id) :
            Rule(id,UUID)
            {
            }

            bool Terminal::admit( YOCTO_LANG_SYNTAX_RULE_ADMIT_ARGS ) const
            {
                Lexeme *lex = lexer.get(source);
                if(lex)
                {
                    if(lex->label==label)
                    {
                        Node::Grow(tree, Node::Create(*this,lex));
                        return true;
                    }
                    else
                    {
                        lexer.unget(lex);
                        return false;
                    }
                }
                else
                {
                    //EOF
                    return false;
                }
            }

            void Terminal:: __viz(ios::ostream &fp) const
            {
                fp.viz(this);
                fp("[shape=box,label='%s'];\n",label.c_str());
            }

        }
        
    }
    
}
