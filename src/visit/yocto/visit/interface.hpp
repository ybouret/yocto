#ifndef YOCTO_VISIT_INTERFACE_INCLUDED
#define YOCTO_VISIT_INTERFACE_INCLUDED 1

#include "VisItControlInterface_V2.h"
#include "VisItDataInterface_V2.h"
#include "yocto/mpi/mpi.hpp"
#include "yocto/threading/singleton.hpp"

namespace yocto
{
    class VisIt : public singleton<VisIt>
    {
    public:
        class Simulation; //!< forward

        const mpi   &MPI;
        
        static VisIt &Start(const string &sim_name,
                            const string &sim_comment,
                            const mpi    &usrMPI);


        
    private:
        YOCTO_DISABLE_COPY_AND_ASSIGN(VisIt);
        friend class singleton<VisIt>;
        virtual ~VisIt() throw();
        explicit VisIt();

        static const threading::longevity life_time = -10;
        static const char name[];

    public:

        class Simulation
        {
        public:
            const mpi &MPI;
            int        runMode;
            int        cycle;
            double     runTime;
            bool       done;

            virtual ~Simulation() throw();
            explicit Simulation( const VisIt &visit );

            void loop();

        private:
            YOCTO_DISABLE_COPY_AND_ASSIGN(Simulation);
        };
    };
}

#endif
