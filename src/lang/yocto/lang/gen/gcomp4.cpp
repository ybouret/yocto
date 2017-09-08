#include "yocto/lang/gen/gcompiler.hpp"
#include "yocto/exception.hpp"

namespace yocto
{
    namespace Lang
    {
        namespace Syntax
        {

            void gCompiler:: finalize()
            {
                if(verbose)
                {
                    std::cerr << "== finalizing " << parser->tag << " ==" << std::endl;
                    std::cerr << "==== checking..." << std::endl;
                }
                parser->check();

                if(verbose)
                {
                    std::cerr << "==== tweaking..." << std::endl;
                }
                unsigned ir = 1;
                for(Rule *r = (Rule *)(parser->getTopLevel());r;r=r->next,++ir)
                {
                    if(verbose)
                    {
                        std::cerr << "checking rule #" << ir << "='" << r->label << "'" << std::endl;
                    }
                    
                    switch(r->uuid)
                    {

                        case Aggregate::UUID:
                        {
                            Aggregate &a = *static_cast<Aggregate *>(r->self_);
                            if(a.members.size==1 && Alternate::UUID==a.members.head->addr->uuid)
                            {
                                if(verbose)
                                {
                                    std::cerr << "\t+IsFusing" << std::endl;
                                }
                                r->let(IsFusing);
                            }
                        } break;

                        default:
                            break;
                    }
                }


            }
        }

    }
}

