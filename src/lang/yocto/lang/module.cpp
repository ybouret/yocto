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

        Module * Module:: OpenFile(const string &id)
        {
            const Input in( new ios::icstream(id) );
            return new Module(id,in);
        }

        Module * Module:: OpenFile(const char *id)
        {
            const Input in( new ios::icstream(id) );
            return new Module(id,in);
        }

        Module * Module:: OpenSTDIN()
        {
            const string id = "STDIN";
            const Input  in( new ios::icstream(ios::cstdin) );
            return new Module(id,in);
        }

        Module * Module:: OpenData(const string &id, const void *buffer, const size_t buflen)
        {
            const Input in(new ios::imstream(buffer,buflen));
            return new Module(id,in);
        }

        Module * Module:: OpenData(const char *id, const void *buffer, const size_t buflen)
        {
            const string ID(id);
            return OpenData(ID,buffer,buflen);
        }

        Module * Module:: OpenData(const string &id, const memory::ro_buffer &buff )
        {
            return OpenData( id, buff.ro(), buff.length() );
        }

        Module * Module:: OpenData(const char *id, const memory::ro_buffer &buff )
        {
            return OpenData( id, buff.ro(), buff.length() );
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
