
#include "yocto/ink/ops/local.hpp"

namespace yocto
{
    namespace Ink
    {
        Local:: ~Local() throw()
        {
        }

        Local:: Local() throw() :
        count(0),
        flags()
        {
        }

        const coord Local::Position[SIZE] =
        {
            coord( 0, 0),  // 0
            coord( 1, 0),  // 1
            coord( 1, 1),  // 2
            coord( 0, 1),  // 3
            coord(-1, 1),  // 4
            coord(-1, 0),  // 5
            coord(-1,-1),  // 6
            coord( 0,-1),  // 7
            coord( 1,-1)   // 8
        };

        const size_t Local::Flags[SIZE] =
        {
            0,
            Right,
            Right | Top,
            Top,
            Left  | Top,
            Left,
            Left | Bottom,
            Bottom,
            Right | Bottom
        };

        size_t Local:: GetPositionIndex(const size_t flag) 
        {
            switch(flag)
            {
                case 0     : return 0;

                case Left  : return 5;
                case Right : return 1;
                case Bottom: return 7;
                case Top   : return 3;

                case Right | Top    : return 2;
                case Left  | Top    : return 4;
                case Right | Bottom : return 8;
                case Left  | Bottom : return 6;
                    
                default:
                    break;
            }
            throw exception("Local:GetPositionIndex(invalid flag=%u)", unsigned(flag) );
        }
    }

}

