
#include "yocto/ink/mask.hpp"


namespace yocto
{
    namespace Ink
    {
        Mask:: ~Mask() throw()
        {
        }
        
        Mask:: Mask() throw() : MaskType() {}
        Mask:: Mask( const Mask &other ) : MaskType(other) {}


        void Mask:: OR( const Mask &other )
        {
            MaskType ans(*this);
            for(const Vertex *v=other.head();v;v=v->next)
            {
                (void) insert(v->pos, new Vertex(*v) );
            }
            swap_with(ans);
        }

        void Mask:: AND( const Mask &other )
        {
            MaskType ans(*this);
            for(const Vertex *lhs = head(); lhs; lhs=lhs->next)
            {
                for(const Vertex *rhs = head(); rhs; rhs=rhs->next)
                {
                    if(lhs->pos==rhs->pos)
                    {
                        (void) insert(lhs->pos, new Vertex( *lhs ) );
                    }
                }
            }
            swap_with(ans);
        }
    }
}
