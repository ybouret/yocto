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


#define LX_PREFIX "@"
#define LX_EXPR   LX_PREFIX ID_EXPR

#define SM_PREFIX "%"
#define SM_EXPR   SM_PREFIX ID_EXPR

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
                Rule &LPAREN  = terminal('(');
                Rule &RPAREN  = terminal(')');

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
                {
                    ALIAS << ID << COLUMN << choice(RX,RS) << END;
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
                        Aggregate &SUB = Decl(agg("SUB"),MergesSingle);
                        SUB <<  oneOrMore(ITEM);

                        //______________________________________________________
                        //
                        // an ALT rule is the choice of different SUB rule
                        //______________________________________________________
                        Aggregate &ALT = Decl(agg("ALT"),MergesSingle);
                        ALT  << SUB  << zeroOrMore( Decl(agg("ALTS"),MergesAlways) << terminal('|') << SUB );

                        //______________________________________________________
                        //
                        // the add the GROUP possibility for an ATOM
                        // GROUP is temporary only, a wrapper for ALT
                        //______________________________________________________
                        ATOM << ( Decl(agg("GROUP"),MergesAlways) << LPAREN << ALT << RPAREN );

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
                // lexical rule : drop, endl, comment
                //______________________________________________________________
                Aggregate &LEXR = agg("LEXR");
                Rule      &LX   = terminal("LX",LX_EXPR);
                {
                    LEXR << LX << COLUMN << zeroOrMore(choice(RX,RS)) << END;
                }

                //______________________________________________________________
                //
                // lexical rule : plugin
                //______________________________________________________________
                Aggregate &PLUG = agg("PLUG");
                {
                    PLUG << LX << COLUMN << ID << END;
                }



                //______________________________________________________________
                //
                // choice for grammar
                //______________________________________________________________
                {
                    Alternate &ENTRIES = alt("ENTRIES");
                    GRAMMAR << zeroOrMore(ENTRIES);
                    ENTRIES << ALIAS << RULE;
                    ENTRIES << LEXR  << PLUG;
#if 1
                    //__________________________________________________________
                    //
                    // Semantic modification rule
                    //__________________________________________________________
                    Aggregate &SEMR = agg("SEMR");
                    {
                        SEMR << terminal("SM",SM_EXPR) << COLUMN << ID;
                        {
                            Aggregate &ARGS = agg("ARGS");
                            ARGS << LPAREN;
                            ARGS << zeroOrMore(ID);
                            ARGS << RPAREN;
                            SEMR << optional(ARGS);

                        }
                        SEMR << END;
                        ENTRIES << SEMR;
                    }
#endif
                }

                //______________________________________________________________
                //
                // lexical only
                //______________________________________________________________
                root.call(hook<Lexical::Comment>("//","//"));
                root.call(hook<Lexical::InlineComment>("/**/","/\\*","\\*/"));
                root.make("ENDL",   "[:endl:]",   YOCTO_LANG_LEXICAL(newline));
                root.make("BLANKS", "[:blank:]+", YOCTO_LANG_LEXICAL(discard));

                if(false)
                {
                    std::cerr << "Saving DynamoParser" << std::endl;
                    graphviz("dynamo.dot");
                    ios::graphviz_render("dynamo.dot");
                }

                check();
            }

            
        }
    }
}
