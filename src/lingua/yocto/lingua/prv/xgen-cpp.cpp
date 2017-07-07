#include "yocto/lingua/prv/xgen-cpp.hpp"
#include "yocto/ios/icstream.hpp"

namespace yocto
{
    namespace lingua
    {
        xgen_cpp:: ~xgen_cpp() throw() {}

        xgen_cpp:: xgen_cpp() : lexer("lingua.prepocessor","root")
        {

        }

        void xgen_cpp:: process( const string &filename, token &content )
        {
            {
                ios::icstream fp(filename);
                char C = 0;
                while( fp.query(C) )
                {
                    content.push_back( new t_char(C) );
                }
            }

            token stk;
            while( content.size )
            {
                const uint8_t c = content.head->code;
                if(c!='#')
                {
                    stk.push_back(content.pop_front());
                    continue;
                }
                delete content.pop_front();
            }

            stk.swap_with(content);

        }


    }
}
