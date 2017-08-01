#include "yocto/lang/pattern/dict.hpp"
#include "yocto/exception.hpp"


#include <iostream>

namespace yocto
{
    namespace Lang
    {
        static const char fn[] = "PattenDict: ";

        PatternDict:: PatternDict() throw()
        {
        }

        PatternDict:: ~PatternDict() throw()
        {
        }

        void PatternDict:: define(const string &id, Pattern *p)
        {
            const PatternPtr q(p);
            if( id.size() <= 0 )            throw exception("%sempty identifier",fn);
            if( ! IsValidFirstChar(id[0]) ) throw exception("%sinvalid identifier '%s'",fn,id.c_str());
            if(!insert(id,q))
            {
                throw exception("%smuliple '%s'", fn,id.c_str());
            }
        }

        void PatternDict:: define(const char *id_, Pattern *p)
        {
            const PatternPtr q(p);
            const string     id(id_);
            if(!insert(id,q))
            {
                throw exception("%smuliple '%s'",fn,id.c_str());
            }
        }

        Pattern * PatternDict:: operator[](const string &id) const
        {
            const PatternPtr *ppPattern = search(id);
            if(!ppPattern)
            {
                throw exception("%sno '%s'",fn,id.c_str());
            }
            return (*ppPattern)->clone();
        }

        Pattern * PatternDict:: operator[](const char *id) const
        {
            const string ID(id);
            return (*this)[ID];
        }

        bool PatternDict:: IsValidFirstChar(const char C) throw()
        {
            if( (C>='a' && C<='z') || (C>='A'&&C<='Z') || (C == '_') )
            {
                return true;
            }
            else
            {
                return false;
            }
        }

    }
}
