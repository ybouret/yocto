#include "yocto/lang/lexical/plugin/string64.hpp"
#include "yocto/lang/lexical/translator.hpp"
#include "yocto/lang/pattern/basic.hpp"
#include "yocto/exception.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Lexical
        {
            String64:: ~String64() throw()
            {
            }

            String64:: String64(const string &id, Translator &trans) :
            Plugin(id,trans),
            b64()
            {
                init();
            }

            String64:: String64(const char *id, Translator &trans) :
            Plugin(id,trans),
            b64()
            {
                init();
            }

            const char *String64:: trigger() const throw()
            {
                return "$";
            }

            void String64:: startUp(const Token &)
            {
                b64.clear();
            }

            Result  String64:: grow(const Token &tkn)
            {
                b64.add(tkn);
                return Discard;
            }

            void String64:: init()
            {


                back(trigger(),this, & String64::quit );
                {
                    const string b64ID = label + ".b64";
                    const string b64RX = "[[A-Z][a-z][0-9]]";
                    make(b64ID,b64RX,this,& String64::grow);
                    {
                        auto_ptr<Pattern> motif( RegExp(b64RX,NULL) );
                        motif->graphviz("b64.dot");
                    }
                }

                {
                    const string blanksID = label + ".blanks";
                    const string blanksRX = "[:blank:]+";
                    make(blanksID,blanksRX,translator, & Translator::newline );
                }

                {
                    const string endlID = label + ".endl";
                    const string endlRX = "[:endl:]";
                    make(endlID,endlRX,translator, & Translator::newline);
                }

                {
                    const string any1ID = label + ".any1";
                    make(any1ID, new Any1(), this, & String64::excp );
                }
            }

            Result  String64:: excp(const Token &tkn)
            {
                assert(tkn.size>=1);
                throw exception("<%s> invalid base64 char '%s'", label.c_str(), tkn.head->text() );
                return Discard;
            }

            void String64::quit(const Token &)
            {
                assert(translator);
                translator->unget( newUnit(b64) );
            }
            
        }
    }
}
