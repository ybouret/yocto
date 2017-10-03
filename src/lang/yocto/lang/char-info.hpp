#ifndef YOCTO_LANG_CHAR_INFO_INCLUDED
#define YOCTO_LANG_CHAR_INFO_INCLUDED 1

#include "yocto/ptr/arc.hpp"
#include "yocto/string.hpp"

namespace yocto
{
    
    namespace Lang
    {

        //! shared string to put a stamp on dynamic objects
        typedef arc_ptr<const string> Stamp;

        //! Char information if multiple sources
        class CharInfo
        {
        public:
            CharInfo(const Stamp &st) throw();
            virtual ~CharInfo() throw();
            CharInfo(const CharInfo &) throw();
            
            const int   column; //!< column in stamp
            const int   line;   //!< line   in stamp
            const Stamp stamp;  //!< stamp  name
            
            void newChar() throw();
            void newLine() throw();
            
        private:
            YOCTO_DISABLE_ASSIGN(CharInfo);
        };
        

    }
    
}

#endif

