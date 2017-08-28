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

            Rule:: Rule(const string  &id,
                        const uint32_t t) :
            next(0),
            prev(0),
            uuid(t),
            label(id)
            {
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
                    delete node;
                }
                else
                {
                    while(node->size()>0)
                    {
                        Restore(lexer,node->remove());
                    }
                }
            }


            void Rule:: __mark( ios::ostream &fp ) const
            {
                fp.viz(this);
            }


        }
    }
}

