#include "yocto/lang/char-info.hpp"

namespace yocto
{
    
    namespace Lang
    {
        
        CharInfo:: ~CharInfo() throw()
        {
        }
        
        CharInfo:: CharInfo(const Stamp &st) throw() :
        column(1),
        line(1),
        stamp(st)
        {
        }
        
        CharInfo:: CharInfo(const CharInfo &ch) throw() :
        column(ch.column),
        line(ch.line),
        stamp(ch.stamp)
        {
        }
        
        void CharInfo:: newchar() throw()
        {
            ++((int &)column);
        }
        
        void CharInfo:: newline() throw()
        {
            ++( (int&) line );
            (int &) column = 1;
        }
        
        
    }
    
}
