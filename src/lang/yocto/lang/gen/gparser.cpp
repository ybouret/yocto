#include "yocto/lang/gen/gparser.hpp"
#include "yocto/lang/lexical/plugin/cstring.hpp"
#include "yocto/lang/lexical/plugin/comment.hpp"
#include "yocto/exception.hpp"

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
#define LEXICAL_NAME_PREFIX "@"
#define LEXICAL_NAME     LEXICAL_NAME_PREFIX EXPR_ID
#define SEMANTIC_NAME_PREFIX "$"
#define SEMANTIC_NAME    SEMANTIC_NAME_PREFIX EXPR_ID

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
                Rule &RX      = term<Lexical::cstring>("RX");
                Rule &RS      = term<Lexical::rstring>("RS");

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

                    //__________________________________________________________
                    //
                    // Rule content
                    //__________________________________________________________
                    {
                        //______________________________________________________
                        //
                        // an ATOM is a basic content
                        //______________________________________________________
                        Alternate &ATOM = alt("ATOM");
                        ATOM << ID << RX << RS;

                        //______________________________________________________
                        //
                        // an ITEM is a modified ATOM or an ATOM
                        //______________________________________________________
                        Alternate &ITEM = alt("ITEM");
                        {
                            ITEM << ( agg("OOM") << ATOM << terminal('+'));
                            ITEM << ( agg("ZOM") << ATOM << terminal('*'));
                            ITEM << ( agg("OPT") << ATOM << terminal('?'));
                            ITEM << ATOM;
                        }

                        //______________________________________________________
                        //
                        // a SUB rule is one or more item
                        //______________________________________________________
                        Aggregate &SUB = agg("SUB").noSingle();
                        SUB <<  oneOrMore(ITEM);

                        //______________________________________________________
                        //
                        // an ALT rule is the choice of different SUB rule
                        //______________________________________________________
                        Aggregate &ALT = agg("ALT").noSingle();
                        ALT  << SUB  << zeroOrMore( agg("ALT#EX") << terminal('|') << SUB );

                        //______________________________________________________
                        //
                        // the add the GROUP possibility for an ATOM
                        // GROUP is temporary only, a wrapper for ALT
                        //______________________________________________________
                        ATOM << ( agg("GROUP").noSingle() << terminal('(') << ALT << terminal(')') );

                        //______________________________________________________
                        //
                        // done
                        //______________________________________________________
                        RULE << ALT;

                    }

                    RULE << END;
                }

                //______________________________________________________________
                //
                // A lexical rule
                //______________________________________________________________
                Aggregate &LXR = agg("LXR");
                LXR << terminal("LX",LEXICAL_NAME) << COLUMN << zeroOrMore(choice(RX,RS)) << END;

                //______________________________________________________________
                //
                // A semantic modifier
                //______________________________________________________________
                Aggregate &SMR = agg("SMR");
                SMR << terminal("SM",SEMANTIC_NAME) << COLUMN << zeroOrMore(ID) << END;
                
                GRAMMAR << oneOrMore( choice(RULE,LXR,SMR) );

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

