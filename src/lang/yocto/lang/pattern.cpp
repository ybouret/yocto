#include "yocto/lang/pattern.hpp"
#include "yocto/ios/ocstream.hpp"

#include "yocto/ios/osstream.hpp"
#include "yocto/string/base64.hpp"

namespace yocto
{
    namespace Lang
    {
        Pattern:: ~Pattern() throw()
        {
        }

        Pattern:: Pattern( const Pattern &p ) throw() :
        uuid(p.uuid),
        next(0),
        prev(0),
        addr(0)
        {
        }

        Pattern:: Pattern(const uint32_t t) throw() :
        uuid(t),
        next(0),
        prev(0),
        addr(0)
        {
        }
        
        void Pattern:: __mark( ios::ostream &fp) const
        {
            fp.viz( this );
        }

        void Pattern:: graphviz(const string &filename) const
        {
            ios::wcstream fp(filename);
            fp << "digraph G {\n";
            __viz(fp);
            fp << "}\n";
        }

        void Pattern:: save( ios::ostream &fp ) const
        {
            fp.emit(uuid);
            __sav(fp);
        }

        string Pattern:: toString() const
        {
            static base64 &Base64 = base64::instance();

            string bin;
            {
                ios::osstream fp(bin);
                save(fp);
            }
            string bin64 = Base64.Encode(bin);
            return bin64;
        }
    }

}

#include "yocto/hashing/sha1.hpp"
#include "yocto/code/utils.hpp"

namespace yocto
{
    namespace Lang
    {
        string Pattern:: signature() const
        {
            string bin;
            {
                ios::osstream fp(bin);
                save(fp);
            }
            uint8_t out[4];
            
            {
                hashing::sha1 H;
                H.set();
                H(bin);
                H.get(out,sizeof(out));
            }

            string ans;

            for(size_t i=0;i<sizeof(out);++i)
            {
                ans.append(hexa_text_lower[out[i]]);
            }

            return ans;
        }


    }
}
