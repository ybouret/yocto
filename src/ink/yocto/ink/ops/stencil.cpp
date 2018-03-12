
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
negative_weight(0),\
positive_weight(0),\
weight(0),                           \
factor(1),                          \
nrm(false), vmin(0), vmax(0)

        Stencil:: Stencil(const string &id, const unit_t w, const unit_t h) :
        YOCTO_INK_STENCIL_CTOR() {}

        Stencil:: Stencil(const char *id, const unit_t w, const unit_t h) :
        YOCTO_INK_STENCIL_CTOR() {}

        void Stencil::compile() throw()
        {
            factor = 1;
            weight = 0;
            positive_weight = 0;
            negative_weight = 0;
            for(size_t i=0;i<count;++i)
            {
                const float w = entry[i];
                if(w>0)
                {
                    positive_weight += w;
                }
                else
                {
                    if(w<0)
                    {
                        negative_weight -= w;
                    }
                    else
                    {
                        entry[i] = 0;
                    }
                }
            }
            weight = positive_weight + negative_weight;
            if(weight>0)
            {
                factor=1.0f/weight;
            }
            std::cerr << "pos=" << positive_weight << ", neg=" << negative_weight << ", weight=" << weight << ", factor=" << factor << std::endl;
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

        void Stencil:: print() const
        {
            std::cerr << name << "={";
            for(size_t i=1;i<=count;++i)
            {
                std::cerr << " " << entry[i-1];
                if(i<count)
                {
                    std::cerr << ",";
                }
            }
            std::cerr << " }" << std::endl;
        }

    }
}
