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

            YOCTO_LANG_SYNTAX_RULE_IMPL(Terminal)
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
                    //EOF;
                    return false;
                }
            }
            YOCTO_LANG_SYNTAX_RULE_DONE()


        }

    }
    
}
