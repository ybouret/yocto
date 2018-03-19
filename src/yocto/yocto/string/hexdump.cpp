
#include "yocto/string/hexdump.hpp"
//#include "yocto/memory/buffer.hpp"

namespace yocto
{


    void hexdump::write(ios::ostream &fp, const char *data, const size_t size)
    {
        static const size_t LINE_SIZE = 16;

        assert( !(NULL==data&&size>0) );
        size_t       count = 0;
        string       info;
        const size_t last  = size-1;
        for(size_t i=0;i<size;++i)
        {
            const char    c = data[i];
            const uint8_t b = uint8_t(c);
            if(b<32||b>=127)
            {
                info << '.';
                fp(" 0x%02x",unsigned(b));
            }
            else
            {
                info << c;
                fp("  '%c'",c);
            }
            if(i<last)
            {
                fp << ',';
            }
            ++count;
            if(count>=LINE_SIZE)
            {
                count = 0;
                fp << "\t/* ";
                fp << info;
                fp << " */\n";
                info.clear();
            }
        }
        if(count>0)
        {
            while(count<LINE_SIZE)
            {
                ++count;
                fp << "      ";
                info << ' ' ;
            }
            fp << " \t/* ";
            fp << info;
            fp << " */\n";
        }
    }

    void hexdump:: write(ios::ostream            &fp,
                         const memory::ro_buffer &buff)
    {
        write(fp,(const char *) buff.ro(), buff.length() );
    }
}
