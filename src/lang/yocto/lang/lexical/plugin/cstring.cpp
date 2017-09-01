#include "yocto/lang/lexical/plugin/cstring.hpp"
#include "yocto/lang/lexical/translator.hpp"
#include "yocto/exception.hpp"
#include "yocto/code/utils.hpp"

#define Y_LANG_CSTRING_OUT 0

#if 1 == Y_LANG_CSTRING_OUT
#include "yocto/ios/graphviz.hpp"
#endif

namespace yocto
{
    namespace Lang
    {
        namespace Lexical
        {

            cstring:: ~cstring() throw()
            {
            }

            cstring:: cstring(const char *id, Translator &trans) :
            Plugin(id,trans),
            data()
            {
                init();
            }

            cstring:: cstring(const string &id, Translator &trans) :
            Plugin(id,trans),
            data()
            {
                init();
            }

            void cstring:: init()
            {
                const string charExpr = "[\\x20-\\x21\\x23-\\x5b\\x5d-\\x7e]";

#if 1 == Y_LANG_CSTRING_OUT
                {
                    auto_ptr<Pattern> p( RegExp(charExpr,NULL) );
                    p->graphviz("cstring.dot");
                    ios::graphviz_render("cstring.dot");
                }
#endif

                const string charMake = label + ".char";


                back( trigger(),  this, & cstring::quit  );
                make(charMake, charExpr, this,  & cstring::grow  );

                const string escCtrlExpr   = "\\x5c[nrbtfav]";
                const string escCtrlMake   = label + ".esc.ctrl";
                make(escCtrlMake,escCtrlExpr,this, & cstring::escCtrl );

                const string escCopyExpr = "\\x5c[\\x22\\x27\\x5c]";
                const string escCopyMake = label + ".esc.copy";
                make(escCopyMake,escCopyExpr,this, & cstring::escCopy);

                const string escHexaExpr = "\\x5cx[:xdigit:][:xdigit:]";
                const string escHexaMake = label + ".esc.hexa";
                make(escHexaMake,escHexaExpr,this, &cstring::escHexa);

                const string escFailureExpr = "\\x5c[:any1:]";
                const string escFailureMake = label + "esc.failure";
                make(escFailureMake,escFailureExpr,this, & cstring::escFailure);

            }

            const char * cstring::trigger() const throw()
            {
                return "\"";
            }

            void cstring:: startUp(const Token &)
            {
                data.clear();
            }

            Result cstring:: grow(const Token &tkn) throw()
            {
                data.add(tkn);
                return Discard;
            }

            void cstring:: quit(const Token &)
            {
                assert(translator);
                translator->unget( newUnit(data) );
            }

            Result cstring:: escCtrl(const Token &tkn)
            {
                assert(2==tkn.size);
                switch(tkn.tail->code)
                {
                    case 'n': data.push_back( tkn.tail->subst('\n') ); break;
                    case 'r': data.push_back( tkn.tail->subst('\r') ); break;
                    case 'b': data.push_back( tkn.tail->subst('\b') ); break;
                    case 't': data.push_back( tkn.tail->subst('\t') ); break;
                    case 'f': data.push_back( tkn.tail->subst('\f') ); break;
                    case 'a': data.push_back( tkn.tail->subst('\a') ); break;
                    case 'v': data.push_back( tkn.tail->subst('\v') ); break;

                    default:
                        // do nothing, for the future ?
                        ;
                }
                return Discard;
            }

            Result cstring:: escCopy(const Token &tkn)
            {
                assert(2==tkn.size);
                data.push_back( new Char( *tkn.tail) );
                return Discard;
            }

            Result cstring:: escHexa(const Token &tkn)
            {
                assert(4==tkn.size);
                const int lo = hex2dec(tkn.tail->code);
                const int hi = hex2dec(tkn.tail->prev->code);
                const int ch = (hi<<4) | lo;
                data.push_back( tkn.head->subst(ch) );
                return Discard;
            }

            Result cstring:: escFailure(const Token &tkn)
            {
                throw exception("%s.%s: invalid escape sequence", translator->name.c_str(), label.c_str());
                return Discard;
            }


        }

    }

}
