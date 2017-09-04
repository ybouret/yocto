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

            _String:: ~_String() throw()
            {
            }

            _String:: _String(const char *id, Translator &trans, const char *ch) :
            Plugin(id,trans),
            data(),
            __ch(ch)
            {
                init();
            }

            _String:: _String(const string &id, Translator &trans,const char *ch) :
            Plugin(id,trans),
            data(),
            __ch(ch)
            {
                init();
            }

            void _String:: init()
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


                back( trigger(),  this, & _String::quit  );
                make(charMake, charExpr, this,  & _String::grow  );

                const string escCtrlExpr   = "\\x5c[nrbtfav]";
                const string escCtrlMake   = label + ".esc.ctrl";
                make(escCtrlMake,escCtrlExpr,this, & _String::escCtrl );

                const string escCopyExpr = "\\x5c[\\x22\\x27\\x5c]";
                const string escCopyMake = label + ".esc.copy";
                make(escCopyMake,escCopyExpr,this, & _String::escCopy);

                const string escHexaExpr = "\\x5cx[:xdigit:][:xdigit:]";
                const string escHexaMake = label + ".esc.hexa";
                make(escHexaMake,escHexaExpr,this, &_String::escHexa);

                const string escFailureExpr = "\\x5c[:any1:]";
                const string escFailureMake = label + "esc.failure";
                make(escFailureMake,escFailureExpr,this, & _String::escFailure);

            }

            const char * _String::trigger() const throw()
            {
                return __ch.c_str();//"\"";
            }

            void _String:: startUp(const Token &)
            {
                data.clear();
            }

            Result _String:: grow(const Token &tkn)
            {
                data.add(tkn);
                return Discard;
            }

            void _String:: quit(const Token &)
            {
                assert(translator);
                translator->unget( newUnit(data) );
            }

            Result _String:: escCtrl(const Token &tkn)
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

            Result _String:: escCopy(const Token &tkn)
            {
                assert(2==tkn.size);
                data.push_back( new Char( *tkn.tail) );
                return Discard;
            }

            Result _String:: escHexa(const Token &tkn)
            {
                assert(4==tkn.size);
                const int lo = hex2dec(tkn.tail->code);
                const int hi = hex2dec(tkn.tail->prev->code);
                const int ch = (hi<<4) | lo;
                data.push_back( tkn.head->subst(ch) );
                return Discard;
            }

            Result _String:: escFailure(const Token &tkn)
            {
                throw exception("%s.%s: invalid escape sequence", translator->name.c_str(), label.c_str());
                return Discard;
            }

            ////////////////////////////////////////////////////////////////////

            const char cstring::Expr[] = "\"";

            cstring:: cstring(const char *id,
                              Translator &trans) :
            _String(id,trans,Expr)
            {
            }

            cstring:: cstring(const string &id,
                              Translator   &trans) :
            _String(id,trans,Expr)
            {
            }

            cstring:: ~cstring() throw() {}


            ////////////////////////////////////////////////////////////////////

            const char rstring::Expr[] = "\'";

            rstring:: rstring(const char *id,
                              Translator &trans) :
            _String(id,trans,Expr)
            {
            }

            rstring:: rstring(const string &id,
                              Translator   &trans) :
            _String(id,trans,Expr)
            {
            }

            rstring:: ~rstring() throw() {}




        }

    }

}
