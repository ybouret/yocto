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

                //______________________________________________________________
                //
                // starting from the first 'RULE' node after parser tag
                //______________________________________________________________
                for(;node;node=node->next)
                {
                    const string &label = node->origin.label;
                    switch(rootHash(label))
                    {
                        case 0: assert( "RULE" == label );  __registerNewRule(node); break;
                        case 1: assert( "LXR"  == label );  __detectPlugin(node);    break;
                        default: break;
                    }
                }

                if(verbose)
                {
                    std::cerr << std::endl;
                }

            }

            int gCompiler::StringKind(const Node *node) const throw()
            {
                return kindHash(node->origin.label);
            }

            void gCompiler:: __registerNewRule(const Node *node)
            {
                static const char fn[] = "gCompiler.registerNewRule: ";
                //______________________________________________________________
                //
                //
                // get the label of the rule
                //
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
                //
                // detect is this is an alias : only one following string
                //
                //______________________________________________________________
                const int stringKind = StringKind(arg);
                if( stringKind>=0 && (NULL==arg->next) )
                {
                    //__________________________________________________________
                    //
                    // this is an alias
                    //__________________________________________________________
                    if(verbose) { std::cerr << "|_alias" << std::endl; }
                    const string expr    = StringToExpr(arg);
                    Terminal    &newTerm = parser->terminal(label,expr);
                    __newTerm(fn,label,&newTerm);
                    if(1==stringKind)
                    {
                        assert("RS"==arg->origin.label);
                        newTerm.let(IsUnique);
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

            void gCompiler::__detectPlugin(const Node *lxr)
            {
                static const char fn[] = "gCompiler.detectPlugin: ";

                //______________________________________________________________
                //
                // get the info
                //______________________________________________________________
                const Node::List   &code = lxr->toList();     assert(code.size>0);
                const Node         *node = code.head;         assert(node!=NULL); assert("LX"==node->origin.label);
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
                node=node->next;   assert(node); assert(StringKind(node)>=0);
                const string pluginName = node->toString();
                if(verbose)
                {
                    std::cerr << '<' << pluginName << '>' << std::endl;
                }

                //______________________________________________________________
                //
                // hardcoded plugins
                //______________________________________________________________
                Terminal *plg = 0;
                if( "cstring" == pluginName )
                {
                    plg = & parser->term<Lexical::cstring>(name);
                    goto REGISTER_TERM;
                }

                if( "rstring" == pluginName )
                {
                    plg = & parser->term<Lexical::rstring>(name);
                    goto REGISTER_TERM;
                }

                throw exception("%splugin '%s' is not implemented", fn, pluginName.c_str() );
                
            REGISTER_TERM:
                assert(NULL!=plg);
                __newTerm(fn,name,plg);
            }
            
        }
    }
}

