#include "yocto/lingua/pattern/matcher.hpp"
#include "yocto/ios/imstream.hpp"
#include "yocto/exception.hpp"

namespace yocto
{
    namespace lingua
    {

        matcher:: matcher(pattern *p): motif(p)
        {
            if(motif->match_empty())
            {
                throw exception("matcher: pattern is accepting empty strings");
            }
        }

        matcher:: ~matcher() throw()
        {
        }

        bool matcher:: full_match(const string &s)
        {
            if(s.size()<=0) return false;
            ios::imstream inp(s);
            source        src(inp);
            last.clear();
            return (motif->match(last,src)&&s.size()==last.size);
        }

        bool matcher:: partial_match(const string &s)
        {
            token best;
            const size_t n       = s.size();
            const char  *q       = s.c_str();
            bool         success = false;

            for(size_t i=0;i<n;++i)
            {
                const char  *org     = q+i;
                const size_t max_sub = n-i;
                for(size_t l=1;l<=max_sub;++l)
                {
                    const string sub(org,l);
                    if(full_match(sub))
                    {
                        success = true;
                        if(last.size>best.size)
                        {
                            best.swap_with(last);
                        }
                    }
                }
            }

            if(success)
            {
                last.swap_with(best);
            }
            return success;
        }


    }

}
