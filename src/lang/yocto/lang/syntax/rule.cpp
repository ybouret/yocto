#include "yocto/lang/syntax/rule.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            Rule:: ~Rule() throw()
            {
            }

#define YOCTO_LANG_SYNTAX_RULE_CTOR() \
next(0),          \
prev(0),          \
uuid(t),          \
label(id),        \
flags(IsNormal),  \
self_(0), tag(0)

            Rule:: Rule(const string  &id,
                        const uint32_t t) :
            YOCTO_LANG_SYNTAX_RULE_CTOR()
            {

            }


            Rule:: Rule(const char    *id,
                        const uint32_t t) :
            YOCTO_LANG_SYNTAX_RULE_CTOR()
            {
                
            }

            void Rule:: let(const uint32_t flag) throw()
            {
                (uint32_t &)flags = flag;
            }


            const string & Rule:: key() const throw()
            {
                return label;
            }


            void Rule:: Restore( Lexer &lexer, Node *node ) throw()
            {
                assert(NULL!=node);
                if(node->terminal)
                {
                    lexer.unget(node->yield());
                }
                else
                {
                    while(node->size()>0)
                    {
                        Restore(lexer,node->remove_tail());
                    }
                }
                delete node;
            }


            void Rule:: __mark( ios::ostream &fp ) const
            {
                fp.viz(this);
            }

            const char * Rule:: __sty() const throw()
            {
                switch(flags)
                {
                    case IsUnique:     return "bold";
                    case IsHollow:     return "dashed";
                    case MergesSingle: return "rounded";
                    case MergesAlways: return "dashed";
                    default:
                        break;
                }
                return "solid";
            }


        }
    }
}

