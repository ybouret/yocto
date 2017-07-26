#ifndef YOCTO_LANG_CHAR_INFO_INCLUDED
#define YOCTO_LANG_CHAR_INFO_INCLUDED 1

#include "yocto/ptr/arc.hpp"
#include "yocto/string.hpp"

namespace yocto
{
    
    namespace Lang
    {
        
        typedef arc_ptr<string> Stamp;
        
        class CharInfo
        {
        public:
            CharInfo(const Stamp &st) throw();
            virtual ~CharInfo() throw();
            CharInfo(const CharInfo &) throw();
            
            const int   column;
            const int   line;
            const Stamp stamp;
            
            void newchar() throw();
            void newline() throw();
            
        private:
            YOCTO_DISABLE_ASSIGN(CharInfo);
        };
        

    }
    
}

#endif

