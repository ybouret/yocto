#include "yocto/lang/syntax/grammar.hpp"
#include "yocto/exception.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {
            Node * Grammar:: accept(Lexer &lexer, Source &source)
            {
                //______________________________________________________________
                //
                // prepare
                //______________________________________________________________
                const char *gramID = name.c_str();
                if(rules.size<=0) throw exception("{%s} no top level rule", gramID);

                Node *tree = 0;
                Rule &top  = * rules.head;

                //______________________________________________________________
                //
                // try to admit top level rule
                //______________________________________________________________
                int depth=0;
                if(!top.admit(tree,lexer,source,depth))
                {
                    //__________________________________________________________
                    //
                    // at this point, there is no syntax error,
                    // some valid tokens may be there
                    //__________________________________________________________
                    assert(NULL==tree);
                    const Lexeme *lex = lexer.peek(source);
                    if(lex)
                    {
                        throw exception("{%s} unexpected \"%s\" for \"%s\"", gramID, lex->id(), top.id());
                    }
                    else
                    {
                        throw exception("{%s} empty content for expected \"%s\"", gramID , top.id());
                    }
                }
                else
                {
                    auto_ptr<Node> guard(tree);
                    const Lexeme *lex  = lexer.peek(source);
                    if(lex)
                    {
                        const string  info = lex->toString();
                        throw exception("{%s} extraneous \"%s\"='%s' after \"%s\"", gramID, lex->id(), info.c_str(), top.id());
                    }

                    return guard.yield();
                }
            }

        }

    }
}

