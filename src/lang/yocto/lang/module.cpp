#include "yocto/lang/module.hpp"
#include "yocto/ios/icstream.hpp"

namespace yocto
{

    namespace Lang
    {
        CharPosition:: CharPosition() throw() :
        column(1),
        line(1)
        {
        }

        CharPosition:: ~CharPosition() throw()
        {
        }

        CharPosition:: CharPosition( const CharPosition &other ) throw() :
        column(other.column),
        line(other.line)
        {

        }

        void CharPosition:: newchar() throw()
        {
            ++((int &)column);
        }

        void CharPosition:: newline() throw()
        {
            ++( (int&) line );
            (int &) column = 1;
        }

    }

}



namespace yocto
{

    namespace Lang
    {

        Module:: ~Module() throw()
        {
        }


        Module:: Module(const string  &id,
                        const Input   &in ) :
        stamp( new string(id) ),
        input(in)
        {
        }
        
#if 0
        module:: module( const string &filename ) :
        name(filename),
        input( new ios::icstream(filename) )
        {
        }
#endif

    }

}
