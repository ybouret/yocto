#include "yocto/lang/module.hpp"
#include "yocto/ios/icstream.hpp"
#include "yocto/ios/imstream.hpp"



namespace yocto
{

    namespace Lang
    {

        Module:: ~Module() throw()
        {
        }


        Module:: Module(const string  &id,
                        const Input   &in ) :
        CharInfo( new string(id) ),
        input(in)
        {
        }
        
        Module:: Module(const string &id) :
        CharInfo( new string(id) ),
        input( new ios::icstream( *stamp) )
        {
        }

        Module:: Module(const void *buffer, const size_t buflen) :
        CharInfo( new string("data") ),
        input( new ios::imstream(buffer,buflen) )
        {
        }

        
        Module:: Module() :
        CharInfo( new string("STDIN") ),
        input( new ios::icstream( ios::cstdin) )
        {
        }

        Char * Module:: get()
        {
            char C = 0;
            ios::istream &in = (ios::istream &)(*input);
            if( in.query(C) )
            {
                Char *ch = new Char(*this,C);
                newChar();
                return ch;
            }
            else
            {
                return NULL;
            }
        }

    }

}
