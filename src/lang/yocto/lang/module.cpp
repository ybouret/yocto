#include "yocto/lang/module.hpp"
#include "yocto/ios/icstream.hpp"

namespace yocto
{

    namespace lang
    {
        t_position:: t_position() throw() :
        column(1),
        line(1)
        {
        }

        t_position:: ~t_position() throw()
        {
        }

        t_position:: t_position( const t_position &other ) throw() :
        column(other.column),
        line(other.line)
        {

        }

        void t_position:: newchar() throw()
        {
            ++((int &)column);
        }

        void t_position:: newline() throw()
        {
            ++( (int&) line );
            (int &) column = 1;
        }

    }

}



namespace yocto
{

    namespace lang
    {

        module:: ~module() throw()
        {
        }


        module:: module( const string &id, const handle &in ) :
        name(id),
        input(in)
        {
        }
        

        module:: module( const string &filename ) :
        name(filename),
        input( new ios::icstream(filename) )
        {
        }

    }

}
