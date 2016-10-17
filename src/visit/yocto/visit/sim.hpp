#ifndef YOCTO_VISIT_SIM_INCLUDED
#define YOCTO_VISIT_SIM_INCLUDED 1

#include "VisItControlInterface_V2.h"
#include "VisItDataInterface_V2.h"
#include "yocto/mpi/mpi.hpp"
#include "yocto/threading/singleton.hpp"

namespace yocto
{
    class VisIt : public singleton<VisIt>
    {
    public:

    private:
        YOCTO_DISABLE_COPY_AND_ASSIGN(VisIt);
        virtual ~VisIt() throw();
        explicit VisIt();
    };
}

#endif
