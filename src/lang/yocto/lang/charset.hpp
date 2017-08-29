#ifndef YOCTO_LANG_CHARSET_INCLUDED
#define YOCTO_LANG_CHARSET_INCLUDED 1

#include "yocto/ordered/sorted-vector.hpp"
#include "yocto/string.hpp"

namespace yocto
{
    namespace Lang
    {
        typedef sorted_vector<uint8_t> _CharSet;

        class CharSet : public _CharSet
        {
        public:
            
            explicit CharSet() throw();
            virtual ~CharSet() throw();
            CharSet( const CharSet &cs );


            CharSet & operator<<( const char    C );
            CharSet & operator<<( const char   *syms );
            CharSet & operator<<( const string &syms);

            
        private:
            YOCTO_DISABLE_ASSIGN(CharSet);
        };

    }
}

#endif
