#include "yocto/lang/dynamo/parser.hpp"
#include "yocto/lang/lexical/plugin/cstring.hpp"
#include "yocto/lang/lexical/plugin/comment.hpp"
#include "yocto/ios/graphviz.hpp"

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
                Rule &END     = terminal(';');
                Rule &ID      = terminal("ID",ID_EXPR);
                Rule &COLUMN  = terminal(':');
                Rule &RX      = term<Lexical::cstring>("RX");
                Rule &RS      = term<Lexical::rstring>("RS");
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
                // A syntax ALIAS
                //______________________________________________________________

                Aggregate &ALIAS = agg("ALIAS");


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
                        Aggregate &SUB = agg("SUB").decl(MergesSingle);
                        SUB <<  oneOrMore(ITEM);

                        //______________________________________________________
                        //
                        // an ALT rule is the choice of different SUB rule
                        //______________________________________________________
                        Aggregate &ALT = agg("ALT").decl(MergesSingle);
                        ALT  << SUB  << zeroOrMore( agg("ALT#EX").decl(MergesAlways) << terminal('|') << SUB );

                        //______________________________________________________
                        //
                        // the add the GROUP possibility for an ATOM
                        // GROUP is temporary only, a wrapper for ALT
                        //______________________________________________________
                        ATOM << ( agg("GROUP").decl(MergesSingle) << terminal('(') << ALT << terminal(')') );

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
                // choice for grammar
                //______________________________________________________________
                {
                    Alternate &ENTRIES = alt("ENTRIES");
                    GRAMMAR << zeroOrMore(ENTRIES);
                    ENTRIES << ALIAS << RULE;
                }

                //______________________________________________________________
                //
                // lexical only
                //______________________________________________________________
                root.call(hook<Lexical::Comment>("//","//"));
                root.call(hook<Lexical::InlineComment>("/**/","/\\*","\\*/"));
                root.make("ENDL",   "[:endl:]",   YOCTO_LANG_LEXICAL(newline));
                root.make("BLANKS", "[:blank:]+", YOCTO_LANG_LEXICAL(discard));

                if(true)
                {
                    std::cerr << "Saving DynamoParser" << std::endl;
                    graphviz("dparser.dot");
                    ios::graphviz_render("dparser.dot");
                }

                check();


            }

        }
    }
}
