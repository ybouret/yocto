#ifndef YOCTO_INK_AREA_INCLUDED
#define YOCTO_INK_AREA_INCLUDED 1

#include "yocto/ink/types.hpp"

namespace yocto
{
    namespace Ink
    {

#define YOCTO_INK_AREA_LIMITS(AREA) \
const unit_t xmin = (AREA).x;       \
const unit_t xmax = (AREA).x_end;   \
const unit_t ymin = (AREA).y;       \
const unit_t ymax = (AREA).y_end;   

        class Area : public object
        {
        public:
            static const unsigned AtLowerX = 0x01;
            static const unsigned AtUpperX = 0x02;
            static const unsigned AtLowerY = 0x04;
            static const unsigned AtUpperY = 0x08;

            const unit_t x;
            const unit_t y;
            const unit_t w;
            const unit_t h;
            const unit_t x_end;
            const unit_t y_end;
            explicit Area( const unit_t X, const unit_t Y, const unit_t W, const unit_t H);
            virtual ~Area() throw();
            Area(const Area  &rect) throw();

            inline bool has( const unit_t X, const unit_t Y) const throw()
            {
                return (X>=x) && (X<=x_end) && (Y>=y) && (Y<=y_end);
            }

            inline bool has(const coord p) const throw() { return has(p.x,p.y); }

            inline bool contains( const Area &sub ) const throw()
            {
                return has(sub.x,sub.y) && has(sub.x_end,sub.y_end);
            }

            inline unsigned xpos(const unit_t X) const throw()
            {
                if(X<=x)
                {
                    return AtLowerX;
                }
                else
                {
                    if(X>=x_end)
                    {
                        return AtUpperX;
                    }
                    else
                    {
                        return 0;
                    }
                }
            }

            inline unsigned ypos(const unit_t Y) const throw()
            {
                if(Y<=y)
                {
                    return AtLowerY;
                }
                else
                {
                    if(Y>=y_end)
                    {
                        return AtUpperY;
                    }
                    else
                    {
                        return 0;
                    }
                }
            }

            
        private:
            YOCTO_DISABLE_ASSIGN(Area);
        };
        
    }
}

#endif

