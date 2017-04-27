#include "yocto/visit/interface.hpp"

namespace yocto
{
    VisIt::Simulation:: ~Simulation() throw()
    {
    }

    void VisIt:: Simulation:: __doNothing() throw() {}
    
    VisIt:: Simulation:: Simulation( const VisIt &visit ) :
    MPI(visit.MPI),
    runMode(VISIT_SIMMODE_STOPPED),
    cycle(0),
    runTime(0),
    done(false),
    connected(false),
    doNothing(this, & Simulation:: __doNothing)
    {
    }

    void VisIt::Simulation:: one_step()
    {
        MPI.Printf0(stderr, "[%s] default one_step #%10d\n",name,cycle);
    }

    void VisIt::Simulation:: step()
    {
        one_step();
        ++cycle;
        update();
    }

    void VisIt::Simulation :: addGenericCommand(visit_handle &md,
                                                const char *  command_name)
    {
        assert(command_name);
        visit_handle cmd = VISIT_INVALID_HANDLE;
        if( VISIT_OKAY != VisIt_CommandMetaData_alloc(&cmd) )
        {
            throw exception("[%s] Visit_CommandMetaData_alloc",name);
        }

        VisIt_CommandMetaData_setName(cmd,command_name);
        VisIt_SimulationMetaData_addGenericCommand(md,cmd);
    }

    void VisIt::Simulation:: update() throw()
    {
        VisItTimeStepChanged();
        VisItUpdatePlots();
    }

}
