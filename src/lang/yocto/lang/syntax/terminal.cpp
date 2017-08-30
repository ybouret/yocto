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
                YOCTO_LANG_SYNTAX_RULE_SELF();
            }

            bool Terminal::admit( YOCTO_LANG_SYNTAX_RULE_ADMIT_ARGS ) const
            {
                //std::cerr << "|_?Terminal '" << label << "'" << std::endl;
                Lexeme *lex = lexer.get(source);
                if(lex)
                {
                    //std::cerr << " |_got '" << *lex << "'" << std::endl;
                    if(lex->label==label)
                    {
                        //std::cerr << "  |_yes" << std::endl;
                        Node::Grow(tree, Node::Create(*this,lex));
                        return true;
                    }
                    else
                    {
                        //std::cerr << "  |_no" << std::endl;
                        lexer.unget(lex);
                        return false;
                    }
                }
                else
                {
                    //EOF
                    //std::cerr << " |_EOF" << std::endl;
                    return false;
                }
            }

            const char * Terminal:: __shp() const throw()
            {
                return "box";
            }

            void Terminal:: __viz(ios::ostream &fp) const
            {
                fp.viz(this);
                fp("[shape=%s,label=\"%s\"];\n",__shp(),label.c_str());
            }

            bool Terminal:: admitsEmpty() const throw()
            {
                return false;
            }

        }
        
    }
    
}
