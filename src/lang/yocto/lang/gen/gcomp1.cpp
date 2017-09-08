#include "yocto/lang/gen/gcompiler.hpp"
#include "yocto/exception.hpp"
#include "yocto/lang/lexical/plugin/cstring.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {


            void gCompiler:: registerTopLevelRules(const Node *node)
            {
                if(verbose)
                {
                    std::cerr << "== register top level rules ==" << std::endl;
                }

                for(;node;node=node->next)
                {
                    const string &label = node->origin.label;
                    switch(rootHash(label))
                    {
                        case 0: assert( "RULE" == label );
                            registerNewRule(node);
                            break;

                        case 1: assert( "LXR" == label );
                            detectPlugin(node);
                            break;

                        default:
                            break;
                    }
                }

                if(verbose)
                {
                    std::cerr << std::endl;
                }

            }

            bool gCompiler::IsString(const Node *node) throw()
            {
                if(node->terminal)
                {
                    const string &label = node->origin.label;
                    return ("RX"==label) || ("RS"==label);
                }
                else
                {
                    return false;
                }
            }

            void gCompiler::registerNewRule(const Node *node)
            {
                static const char fn[] = "gCompiler.registerNewRule: ";
                //______________________________________________________________
                //
                // get the label of the rule
                //______________________________________________________________
                const Node::List &children = node->toList();  assert(children.size>0);
                const Node       *child    = children.head;   assert(child->terminal); assert("ID"==child->origin.label);
                const string      label    = child->toString();
                if(verbose)
                {
                    std::cerr << "+RULE " << label << std::endl;
                }

                const Node *arg = child->next;
                if(!arg) throw exception("%sunexpected empty rule '%s' content!",fn, *label);

                //______________________________________________________________
                //
                // detect is this is an alias
                //______________________________________________________________
                if( (NULL==arg->next) && IsString(arg) )
                {
                    if(verbose) { std::cerr << "|_ALIAS" << std::endl; }
                    const string expr    = StringToExpr(arg);
                    Terminal    &newTerm = parser->terminal(label,expr);
                    if(!termDB.insert(label,&newTerm))
                    {
                        throw exception("%sunexpected failure for terminal '%s'",fn,*label);
                    }
                }
                else
                {
                    //__________________________________________________________
                    //
                    // create and register a new rule
                    //__________________________________________________________
                    Aggregate &newRule = parser->agg(label);
                    if( !ruleDB.insert(label,&newRule) )
                    {
                        throw exception("%sunexpected failure for rule '%s'",fn,*label);
                    }
                }
            }

            void gCompiler::detectPlugin(const Node *lxr)
            {
                static const char fn[] = "gCompiler.detectPlugin: ";

                //______________________________________________________________
                //
                // get the info
                //______________________________________________________________

                const Node::List   &code = lxr->toList(); assert(code.size>0);
                const Node         *node = code.head;     assert(node!=NULL); assert("LX"==node->origin.label);
                const string        name = node->toString(1);
                if(verbose)
                {
                    std::cerr << "+LXR " << name << ':';
                }

                //______________________________________________________________
                //
                // check for reserved word
                //______________________________________________________________
                if( lexrHash(name) >= 0)
                {
                    if(verbose)
                    {
                        std::cerr << " (postpone)" << std::endl;
                    }
                    return; //! a reserved word
                }

                //______________________________________________________________
                //
                // check syntax
                //______________________________________________________________
                if(verbose)
                {
                    std::cerr << " plugin ";
                }
                if(code.size!=2)
                {
                    throw exception("%s@%s must have exactly ONE plugin name",fn,name.c_str());
                }

                //______________________________________________________________
                //
                // get the plugin name
                //______________________________________________________________
                node=node->next;   assert("RX"==node->origin.label||"RS"==node->origin.label);
                const string pluginName = node->toString();
                if(verbose)
                {
                    std::cerr << '<' << pluginName << '>' << std::endl;
                }

                //______________________________________________________________
                //
                // hardcoded plugins
                //______________________________________________________________
                Terminal *t = 0;
                if( "cstring" == pluginName )
                {
                    t = & parser->term<Lexical::cstring>(name);
                    goto REGISTER_TERM;
                }

                if( "rstring" == pluginName )
                {
                    t = & parser->term<Lexical::rstring>(name);
                    goto REGISTER_TERM;
                }

                throw exception("%splugin '%s' is not implemented", fn, pluginName.c_str() );
                
            REGISTER_TERM:
                assert(NULL!=t);
                if(!termDB.insert(name,t))
                {
                    throw exception("%sunexpected failure for '%s'",fn,name.c_str());
                }
            }
            
        }
    }
}

