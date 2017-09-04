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
                Rule &REGEXP  = term<Lexical::cstring>("REGEXP");
                Rule &RAWSTR  = term<Lexical::rstring>("RAWSTR");

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

                    {
                        Alternate &ATOM = alt();
                        ATOM << ID << REGEXP << RAWSTR;

                        Aggregate &ITEM = agg("ITEM");
                        ITEM << ATOM;
                        {
                            Alternate &MODIFIER = alt("MODIFIER");
                            MODIFIER << terminal('+').let(IsUnique) << terminal('*').let(IsUnique) << terminal('?').let(IsUnique);
                            ITEM     << optional(MODIFIER);
                        }

                        Aggregate &SUB = agg("SUB");
                        Aggregate &ALT = agg("ALT");
                        ALT  << SUB  << zeroOrMore( agg("EXTRA_ALT") << terminal('|') << SUB );
                        SUB <<  oneOrMore(ITEM);
                        ATOM << ( agg("GRP") << terminal('(') << ALT << terminal(')') );
                        
                        RULE << ALT;

                    }


                    RULE << END;
                }

                GRAMMAR << oneOrMore(RULE);

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

