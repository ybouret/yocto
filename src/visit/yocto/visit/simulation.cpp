#include "yocto/visit/interface.hpp"

namespace yocto
{
    VisIt::Simulation:: ~Simulation() throw()
    {
    }

    void VisIt:: Simulation:: do_nope(YOCTO_VISIT_SIMULATION_CALLBACK_PROTO) throw()
    {
        (void)cmd;
        (void)args;
    }
    
    VisIt:: Simulation:: Simulation( const VisIt &visit ) :
    MPI(visit.MPI),
    runMode(VISIT_SIMMODE_STOPPED),
    cycle(0),
    runTime(0),
    done(false),
    connected(false),
    doNothing(this, & Simulation:: do_nope),
    callbacks(16,as_capacity)
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



    void VisIt::Simulation :: addGenericCommand(visit_handle   &md,
                                                const char *    command_name)
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

    void VisIt::Simulation :: addGenericCommand(visit_handle   &md,
                                                const char *    command_name,
                                                const Callback &cb)
    {
        const string cmd = command_name;
        if(!callbacks.insert(cmd,cb))
        {
            throw exception("%s: multiple callbacks for command '%s'", name, cmd.c_str());
        }
        MPI.Printf0(stderr, "registering <%s>\n", cmd.c_str());
        addGenericCommand(md,cmd.c_str());
    }



    void VisIt::Simulation:: update() throw()
    {
        VisItTimeStepChanged();
        VisItUpdatePlots();
    }

}
