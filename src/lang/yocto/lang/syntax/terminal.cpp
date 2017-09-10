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
                YOCTO_LANG_SYNTAX_RULE_SELF(Terminal);
            }

            bool Terminal::admit( YOCTO_LANG_SYNTAX_RULE_ADMIT_ARGS ) const
            {
                YOCTO_LANG_SYNTAX(std::cerr << "<?TERM '" << label << "'>" << std::endl;);
                Lexeme *lex = lexer.get(source);
                if(lex)
                {
                    if(lex->label==label)
                    {
                        YOCTO_LANG_SYNTAX(std::cerr << "<+TERM '" << label << "'='" << *lex << "'>" << std::endl);
                        Node::Grow(tree, Node::Create(*this,lex));
                        return true;
                    }
                    else
                    {
                        YOCTO_LANG_SYNTAX(std::cerr << "<-TERM '" << label << "'>" << std::endl);
                        lexer.unget(lex);
                        return false;
                    }
                }
                else
                {
                    //EOF
                    YOCTO_LANG_SYNTAX(std::cerr << "<EOF>" << std::endl);
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

            void Terminal:: __lnk(ios::ostream &) const
            {
                // do nothing
            }

            bool Terminal:: admitsEmpty() const throw()
            {
                return false;
            }

        }
        
    }
    
}
