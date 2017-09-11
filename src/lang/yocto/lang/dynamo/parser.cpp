#include "yocto/lang/dynamo/parser.hpp"
#include "yocto/lang/lexical/plugin/comment.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

#define ID_EXPR "[_[:alpha:]][:word:]*"

#define NAME_PREFIX "\\."
#define NAME_EXPR NAME_PREFIX ID_EXPR


            DynamoParser:: ~DynamoParser() throw()
            {
            }

            DynamoParser:: DynamoParser() :
            Parser("Dynamo")
            {
                //______________________________________________________________
                //
                // top-level rule
                //______________________________________________________________
                Aggregate &GRAMMAR = agg("GRAMMAR");

                //______________________________________________________________
                //
                // Common terminals
                //______________________________________________________________
                //Rule &COLUMN = terminal(':');
                Rule &END    = terminal(';');

                //______________________________________________________________
                //
                // parser name declaration
                //______________________________________________________________
                {
                    Aggregate &NAME_DECL = Decl(agg("NAME_DECL"),MergesAlways);
                    NAME_DECL << terminal("NAME",NAME_EXPR) << END;
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

                check();
            }

        }
    }
}
