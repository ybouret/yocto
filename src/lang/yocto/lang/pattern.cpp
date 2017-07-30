#include "yocto/lang/pattern.hpp"
#include "yocto/ios/ocstream.hpp"

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


    }
}
