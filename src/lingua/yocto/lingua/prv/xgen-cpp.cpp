#include "yocto/lingua/prv/xgen-cpp.hpp"
#include "yocto/ios/icstream.hpp"
#include "yocto/lingua/lexical/plugin/cstring.hpp"
#include "yocto/lingua/lexical/plugin/rstring.hpp"

namespace yocto
{
    namespace lingua
    {
        xgen_cpp:: ~xgen_cpp() throw() {}
        
        static const char * kw[] =
        {
            "include", // 0
            "cstring", // 1
            "endl",    // 2
            "blanks",  // 3
            "text"     // 4
        };
        
#define XPP_INCLUDE 0
#define XPP_CSTRING 1
#define XPP_ENDL    2
#define XPP_BLANKS  3
#define XPP_TEXT    4
        
        xgen_cpp:: xgen_cpp() : lexer("lingua.prepocessor","root"),
        hkw(kw,sizeof(kw)/sizeof(kw[0]))
        {
            
#if 1
            root.emit("include","#include");
            lexical::plugin &str = load<lexical::cstring>("cstring");
            str.hook(root);
            root.emit("endl","[:endl:]");
            root.emit("blanks","[:blank:]+");
            root.emit("text","[[:word:][:punct2:]']+");
#endif
            
        }
        
        void xgen_cpp:: process( ios::istream &fp, l_list &lexemes )
        {
            l_list  lxm;
            // first pass: lexical scanning of file
            {
                lexeme *lxr=0;
                source  src(fp);
                while( 0 != ( lxr = get(src) ) )
                {
                    lxm.push_back(lxr);
                    switch( hkw(lxr->label) )
                    {
                        case XPP_CSTRING: assert("cstring"== lxr->label); break;
                        case XPP_TEXT:    assert("text"   == lxr->label); break;
                            
                        default:
                            lxr->clear();
                    }
                }
            }
            
            
            
            // second pass: cleanup
            {
                l_list stk;
                while(lxm.size)
                {
                    lexeme *node = lxm.pop_front();
                    switch( hkw(node->label) )
                    {
                        case XPP_INCLUDE:
                            if(lxm.head&&XPP_BLANKS==hkw(lxm.head->label))
                            {
                                delete lxm.pop_front();
                            }
                            stk.push_back(node);
                            break;
                     
                        case XPP_BLANKS:
                            if(lxm.head&&XPP_ENDL==hkw(lxm.head->label))
                            {
                                delete node;
                                stk.push_back(lxm.pop_front());
                            }
                            else
                            {
                                 stk.push_back(node);
                            }
                            break;
                            
                        default:
                            stk.push_back(node);
                    }
                }
                stk.swap_with(lxm);
            }
            
            
            // third pass: include
            
            
#if 1
            for(const lexeme *node=lxm.head;node;node=node->next)
            {
                switch( hkw(node->label) )
                {
                    case XPP_INCLUDE: std::cerr << "@INCLUDE"; break;
                    case XPP_CSTRING: {
                        const string s = node->to_cstring();
                        std::cerr << '"' << s << '"';
                    } break;
                    case XPP_ENDL:   std::cerr << '\n'; break;
                    case XPP_BLANKS: std::cerr << ' ';  break;
                    case 4: {
                        const string s = node->to_string();
                        std::cerr << s;
                    } break;
                        
                }
                
            }
#endif
            
        }
        
        
    }
}
