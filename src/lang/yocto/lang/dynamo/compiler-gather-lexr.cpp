#include "yocto/lang/dynamo/compiler.hpp"
#include "yocto/exception.hpp"
#include "yocto/lang/lexical/plugin/comment.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            void DynamoCompiler:: lexr(const Node &node)
            {
                static const char fn[] = "DynamoCompiler.lexr: ";
                assert("LEXR"==node.origin.label);
                const Node::List &children = node.toList(); assert(children.size>0);
                const Node       *child    = children.head; assert(child!=NULL);
                const string      lexrKey  = child->toString(1);
                if(verbose) { std::cerr << '@' << lexrKey << ':'; }
                const Node  *arg  = child->next;
                const size_t args = children.size-1;

                switch(lxrHash(lexrKey))
                {
                        //______________________________________________________
                        //
                        //
                    case 0: assert("drop"==lexrKey); {
                        //
                        //______________________________________________________
                        for(;arg;arg=arg->next)
                        {
                            const string ruleExpr = String2Expr(arg);
                            const string ruleName = parser->tag + ".drop@" + ruleExpr;
                            if(verbose) { std::cerr << ' ' << ruleName; }
                            parser->root.make(ruleName,ruleExpr,& *parser, & Lexical::Translator::discard);
                        }
                    } break;

                        //______________________________________________________
                        //
                        //
                    case 1: assert("endl"==lexrKey);
                        //
                        //______________________________________________________
                        for(;arg;arg=arg->next)
                        {
                            const string ruleExpr = String2Expr(arg);
                            const string ruleName = parser->tag + ".endl@" + ruleExpr;
                            if(verbose) { std::cerr << ' ' << ruleName; }
                            parser->root.make(ruleName,ruleExpr,& *parser, & Lexical::Translator::newline);
                        }

                        break;

                        //______________________________________________________
                        //
                        //
                    case 2: assert("comment"==lexrKey);
                        //
                        //______________________________________________________
                        switch(args)
                    {
                            //__________________________________________________
                            //
                        case 1: {
                            //__________________________________________________
                            const string ruleExpr = String2Expr(arg);
                            const string ruleName = parser->tag + ".comment@" + ruleExpr;
                            parser->root.call(parser->hook<Lexical::Comment>(ruleName,ruleExpr));
                            if(verbose) { std::cerr << ' ' << ruleName; }
                        } break;

                            //__________________________________________________
                            //
                        case 2: {
                            //__________________________________________________
                            const string ruleEnter = String2Expr(arg);
                            const string ruleLeave = String2Expr(arg->next);
                            const string ruleName = parser->tag + ".comment@" + ruleEnter + "@" << ruleLeave;
                            parser->root.call(parser->hook<Lexical::InlineComment>(ruleName,ruleEnter,ruleLeave));
                            if(verbose) { std::cerr << ' ' << ruleName; }
                        } break;

                        default:
                            throw exception("%s: comment requires 1 or 2 arguments",fn);
                    }
                        break;


                    default:
                        throw exception("%sunhandled lexical rule '%s'", fn, *lexrKey);
                }
                
                
                if(verbose) { std::cerr << ';' << std::endl; }
            }
            
        }
        
    }
    
}
