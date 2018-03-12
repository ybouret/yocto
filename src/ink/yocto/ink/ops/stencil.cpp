
#include "yocto/ink/ops/stencil.hpp"

namespace yocto
{
    namespace Ink
    {
        Stencil:: ~Stencil() throw()
        {
        }

#define YOCTO_INK_STENCIL_CTOR()        \
StencilType(id,coord(-w,-h),coord(w,h)), \
dx(upper.x),                             \
dy(upper.y),                            \
tgt(0),                                \
src(0),                               \
weight(0),                           \
factor(1),                          \
nrm(false)

        Stencil:: Stencil(const string &id, const unit_t w, const unit_t h) :
        YOCTO_INK_STENCIL_CTOR() {}

        Stencil:: Stencil(const char *id, const unit_t w, const unit_t h) :
        YOCTO_INK_STENCIL_CTOR() {}

        void Stencil::compile() throw()
        {
            factor = 1;
            weight = 0;
            for(size_t i=0;i<count;++i)
            {
                weight += math::Fabs( entry[i] );
            }
            if(weight>0)
            {
                factor=1.0f/weight;
            }
            std::cerr << "weight=" << weight << ", factor=" << factor << std::endl;
        }

        void Stencil::display() const
        {
            const StencilType &self = *this;
            std::cerr << name << ":" << std::endl;
            for(unit_t y=upper.y;y>=lower.y;--y)
            {
                for(unit_t x=lower.x;x<=upper.x;++x)
                {
                    std::cerr << " " << self[y][x];
                }
                std::cerr << std::endl;
            }
        }
    }
}
