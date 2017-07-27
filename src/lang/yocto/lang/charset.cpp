#include "yocto/lang/charset.hpp"
#include "yocto/code/utils.hpp"

namespace yocto
{
    namespace Lang
    {
        CharSet:: ~CharSet() throw() {}

        CharSet:: CharSet(const CharSet &cs ) :
        _CharSet(cs)
        {
        }

        CharSet:: CharSet() throw() : _CharSet() {}


        CharSet & CharSet:: operator<<( const char C )
        {
            (void) insert(C);
            return *this;
        }


        CharSet & CharSet:: operator<<( const char *txt )
        {
            const size_t n = length_of(txt);
            ensure(min_of<size_t>(size()+n,256));
            for(size_t i=0;i<n;++i)
            {
                (void) insert(txt[i]);
            }
            return *this;
        }


        CharSet & CharSet:: operator<<( const string &syms )
        {
            const size_t n = syms.size();
            ensure(min_of<size_t>(size()+n,256));
            for(size_t i=0;i<n;++i)
            {
                (void) insert(syms[i]);
            }

            return *this;
        }
    }
}
