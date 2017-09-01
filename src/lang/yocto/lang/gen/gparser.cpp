#include "yocto/lang/gen/gparser.hpp"
#include "yocto/lang/lexical/plugin/cstring.hpp"
#include "yocto/lang/lexical/plugin/comment.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            gParser:: ~gParser() throw()
            {
            }

#define EXPR_ID          "[_[:alpha:]][:word:]*"
#define EXPR_NAME_PREFIX "\\."
#define EXPR_NAME        EXPR_NAME_PREFIX EXPR_ID

            gParser:: gParser() :
            Parser("gParser")
            {
                //______________________________________________________________
                //
                // Top Level Rule : GRAMMAR
                //______________________________________________________________
                Aggregate &GRAMMAR = agg("GRAMMAR");



                //______________________________________________________________
                //
                // Common Terminals
                //______________________________________________________________
                Rule &END     = terminal(';');
                Rule &ID      = terminal("ID",EXPR_ID);
                Rule &COLUMN  = terminal(':');
                //Rule &LBRACE  = terminal('(');
                //Rule &RBRACE  = terminal(')');
                Rule &REGEXP  = term<Lexical::cstring>("REGEXP");
                Rule &RAWSTR  = term<Lexical::rstring>("RAWSTR");
                //Rule &ALT     = terminal('|');

                //______________________________________________________________
                //
                // Parser name
                //______________________________________________________________
                {
                    Aggregate &NAME_DECL = agg("NAME#DECL");
                    NAME_DECL << terminal("NAME",EXPR_NAME) << END;
                    GRAMMAR << NAME_DECL;
                }

                //______________________________________________________________
                //
                // A syntax rule
                //______________________________________________________________
                Aggregate &RULE = agg("RULE");
                {
                    RULE << ID << COLUMN;
#if 0
                    {



                        Alt &ATOM = alt();
                        ATOM << ID << RXP << RAW;

                        Agg &ITEM = agg("ITEM",property::noSingle);
                        ITEM << ATOM;
                        {
                            Alt &MODIFIER = alt();
                            MODIFIER <<univocal('+') << univocal('*') << univocal('?');
                            ITEM << opt(MODIFIER);
                        }

                        Agg &SUB = agg("SUB",property::noSingle);
                        Agg &ALT = agg("ALT",property::noSingle);
                        ALT  << SUB  << zero_or_more( agg("EXTRA_ALT",property::jettison) << ALTERN << SUB );
                        SUB << one_or_more(ITEM);
                        ATOM << ( agg("GRP",property::jettison) << LPAREN << ALT << RPAREN );
                        
                        RULE << ALT;
                    }
#endif

#if 1
                    {
                        Alternate &ATOM = alt("ATOM");
                        ATOM << ID << REGEXP << RAWSTR;

                        Aggregate &ITEM = agg("ITEM");
                        ITEM << ATOM << Choice( terminal('+'), terminal('*'), terminal('?'));
                        Aggregate &SUB = agg("SUB");
                        Aggregate &ALT = agg("ALT");
                        ALT << SUB << ZeroOrMore( agg("ALT#EX") << terminal('|') << SUB );
                        SUB << OneOrMore(ITEM);
                        ATOM << ( agg("GRP") << terminal('(') << ALT << terminal(')') );

                        //RULE << ALT;
                        RULE << ZeroOrMore(ID);
                    }
#endif
                    
                    RULE << END;
                }

                GRAMMAR << ZeroOrMore(RULE);

                //______________________________________________________________
                //
                // lexical only
                //______________________________________________________________
                root.call(hook<Lexical::Comment>("//","//"));
                root.call(hook<Lexical::InlineComment>("/**/","/\\*","\\*/"));
                root.make("ENDL",   "[:endl:]",   YOCTO_LANG_LEXICAL(newline));
                root.make("BLANKS", "[:blank:]+", YOCTO_LANG_LEXICAL(discard));


                //compile();
            }

            
        }
        
    }
}

