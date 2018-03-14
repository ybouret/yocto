
#include "yocto/ink/ops/gradients.hpp"

namespace yocto
{
    namespace Ink
    {

        Gradients:: ~Gradients() throw()
        {
        }
        
        Gradients:: Gradients( const Bitmap &bmp ) :
        Area(bmp),
        fields(4,w,h),
        //gx( *fields[1] ),
        //gy( *fields[2] ),
        value( *fields[1] ),
        angle( *fields[2] ),
        v_max(0),
        src(0)
        {
        }
        
        void Gradients:: compute(const pixmap   &source,
                                 const Stencil  &dx,
                                 const Stencil  &dy,
                                 Engine         &engine,
                                 const bool      normalize)
        {
            src = &source;
            sdx = &dx;
            sdy = &dy;
            engine.prepare(BytesPerDomain);
            engine.submit_no_flush(this, & Gradients::computeThread );
            v_max = 0;
            engine.flush();
            for(const Domain *dom = engine.head(); dom; dom=dom->next)
            {
                const float Gmax = dom->get<float>(0);
                if(Gmax>v_max)
                {
                    v_max = Gmax;
                }
            }
            if(normalize&&v_max>0)
            {
                engine.submit(this, & Gradients::normalizeThread );
                v_max = 1;
            }
        }
        
        void Gradients:: computeThread(const Domain &dom, threading::context &) throw()
        {
            const pixmap &pxm  = *src;
            float         Gmax = 0.0f;
            YOCTO_INK_AREA_LIMITS(dom);
            for(unit_t j=ymax;j>=ymin;--j)
            {
                //row &gx_j = gx[j];
                //row &gy_j = gy[j];
                row &value_j = value[j];
                row &angle_j = angle[j];
                for(unit_t i=xmax;i>=xmin;--i)
                {
                    float Gx=0.0f, Gy=0.0f;
                    Stencil::compute(Gx, *sdx, Gy, *sdy, pxm, i, j);
                    //gx_j[i] = Gx;
                    //gy_j[i] = Gy;
                    const float Gn = sqrtf(Gx*Gx+Gy*Gy);
                    value_j[i] = Gn;
                    if(Gn>Gmax) Gmax=Gn;
                    angle_j[i] = math::Atan2(Gy,Gx);
                }
            }
            dom.get<float>(0) = Gmax;
        }
        
        void Gradients:: normalizeThread(const Domain &dom, threading::context &) throw()
        {
            YOCTO_INK_AREA_LIMITS(dom);
            for(unit_t j=ymax;j>=ymin;--j)
            {
                row &value_j = value[j];
                for(unit_t i=xmax;i>=xmin;--i)
                {
                    value_j[i] /= v_max;
                }
            }
        }
        
        
    }
    
}

