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

                //Rule &ID      = terminal("ID",EXPR_ID);
                //Rule &COLUMN  = terminal(':').let(IsHollow);

                //______________________________________________________________
                //
                // Common Terminals
                //______________________________________________________________
                Rule &END     = terminal(';').let(IsHollow);




                {
                    Aggregate &NAME_DECL = agg("NAME#DECL");
                    NAME_DECL << terminal("NAME",EXPR_NAME) << END;
                    GRAMMAR << NAME_DECL;
                }


                //______________________________________________________________
                //
                // lexical only
                //______________________________________________________________
                root.call(hook<Lexical::Comment>("//","//"));
                root.call(hook<Lexical::InlineComment>("/**/","/\\*","\\*/"));
                root.make("ENDL",   "[:endl:]",   YOCTO_LANG_LEXICAL(newline));
                root.make("BLANKS", "[:blank:]+", YOCTO_LANG_LEXICAL(discard));


                compile();
            }

            
        }
        
    }
}

