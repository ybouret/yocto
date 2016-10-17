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
        int          cycle;
        double       time;
        int          runMode;
        int          done;
        const int    par_rank;
        const int    par_size;
        const bool   parallel;
        const bool   par_main;

        static VisIt &Start(const string &sim_name,
                            const string &sim_comment,
                            const mpi    &MPI);

    private:
        YOCTO_DISABLE_COPY_AND_ASSIGN(VisIt);
        friend class singleton<VisIt>;
        virtual ~VisIt() throw();
        explicit VisIt();

        static const threading::longevity life_time = -10;
        static const char name[];
    };
}

#endif
