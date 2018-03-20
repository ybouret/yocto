#include "yocto/lang/dynamo/compiler.hpp"
#include "yocto/ios/graphviz.hpp"
#include "yocto/exception.hpp"
#include "yocto/fs/vfs.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {
            void DynamoCompiler:: checkIncludes(Node      *master)
            {
                static const char fn[] = "DynamoCompiler.checkIncludes: ";
                assert(master);
                assert(master->internal);
                Node::List &topLevel = master->toList(); assert(topLevel.size>0);
                Node::List  tmp;
                
                //______________________________________________________________
                //
                //
                // loop over top level input
                //
                //______________________________________________________________
                while(topLevel.size>0)
                {
                    Node *node = topLevel.pop_front();
                    tmp.push_back(node);
                    if("LEXR"==node->origin.label)
                    {
                        //______________________________________________________
                        //
                        // check for LEXR @include
                        //______________________________________________________
                        const Node::List &children = node->toList();     assert(children.size>0);
                        const Node       *child    = children.head;      assert(child!=NULL);
                        const string      lexrKey  = child->toString(1);
                        if("include"==lexrKey)
                        {
                            //__________________________________________________
                            //
                            // identify the file to include
                            //__________________________________________________
                            const size_t args = children.size-1;
                            if(1!=args) throw exception("%smust have only one argument, got %u",fn,unsigned(args));
                            const Node   *arg    = child->next;
                            const Lexeme &lex    = arg->toLex();
                            const string modName = lex.moduleName();
                            const string modDir  = vfs::get_file_dir(modName);
                            const string incFile = lex.toString();
                            const string incPath = modDir + incFile;
                            if(verbose)
                            {
                                std::cerr << "@include <<" << incPath << ">>" << std::endl;
                            }
                            
                            //__________________________________________________
                            //
                            // remove the @include node
                            //__________________________________________________
                            delete tmp.pop_back();
                            
                            //__________________________________________________
                            //
                            // recursive call
                            //__________________________________________________
                            Source               source( Module::OpenFile(incPath) );
                            reset();
                            auto_ptr<Node> inc(parse(source));
                            if(!inc.is_valid()) throw exception("%sunexpected NULL parsed file",fn);
                            
                            Node::List &content = inc->toList(); assert(content.size>0);
                            // remove the nameNode
                            auto_ptr<Node> nameNode( content.pop_front() );
                            const string   theName   = nameNode->toString(1);
                            const Tag      tag( new string(theName) );
                            inc->propagate(tag);
                            checkIncludes( inc.__get() );
                            
                            // and append the parsed content to the local tree
                            tmp.merge_back(content);
                        }
                    }
                }
                topLevel.swap_with(tmp);
            }
            
        }
    }
}
