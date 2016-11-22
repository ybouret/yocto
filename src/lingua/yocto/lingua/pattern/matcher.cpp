#include "yocto/lingua/pattern/matcher.hpp"
#include "yocto/ios/imstream.hpp"

namespace yocto
{
    namespace lingua
    {

        matcher:: matcher(pattern *p) throw() : motif(p)
        {
        }

        matcher:: ~matcher() throw()
        {
        }

        bool matcher:: full_match(const string &s)
        {
            ios::imstream inp(s);
            source        src(inp);
            last.clear();
            return (motif->match(last,src)&&s.size()==last.size);
        }

    }

}
