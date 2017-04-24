#include "yocto/visit/interface.hpp"

namespace yocto
{

    static const char fn[] = "[VISIT] ";

#define VISIT_COMMAND_FAILURE (-1)
#define VISIT_COMMAND_SUCCESS (0)
#define VISIT_COMMAND_PROCESS (1)

    static inline
    void BroadcastSlaveCommand(int *command)
    {
        assert(command);
        if( MPI_SUCCESS != MPI_Bcast(command, 1, MPI_INT, 0, MPI_COMM_WORLD) )
        {
            throw exception("%sBroadcastSlaveCommand",fn);
        }
    }

    static inline
    void SlaveProcessCallback()
    {
        int command = VISIT_COMMAND_PROCESS;
        BroadcastSlaveCommand(&command);
    }

    // dispatch Viewer's command to CPU
    static inline
    bool ProcessVisItCommand( VisIt::Simulation &sim )
    {

        int command = 0;
        if(sim.MPI.IsFirst)
        {
            const int success = VisItProcessEngineCommand();
            if(success)
            {
                command = VISIT_COMMAND_SUCCESS;
                BroadcastSlaveCommand(&command);
                return true;
            }
            else
            {
                command = VISIT_COMMAND_FAILURE;
                BroadcastSlaveCommand(&command);
                return false;
            }
        }
        else
        {
            while(true)
            {
                BroadcastSlaveCommand(&command);
                switch(command)
                {
                    case VISIT_COMMAND_SUCCESS: return true;
                    case VISIT_COMMAND_FAILURE: return false;
                    case VISIT_COMMAND_PROCESS: VisItProcessEngineCommand(); break;
                    default:
                        throw exception("%sunexpected command=%d",fn,command);
                }
            }
        }
        // never get here...
        return true;
    }

    void VisIt:: Simulation:: loop()
    {

        while(!done)
        {
            const int blocking   = (VISIT_SIMMODE_STOPPED == runMode) ? 1 : 0;
            int       visitstate = 0;

            MPI.Printf0(stderr, "%sblocking=%d\n",fn,blocking);

            //__________________________________________________________________
            //
            //
            // get input from VisIt or timeout
            //
            //__________________________________________________________________

            if(MPI.IsFirst)
            {
                assert(0==MPI.CommWorldRank);
                //______________________________________________________________
                //
                // read status from main
                //______________________________________________________________
                visitstate = VisItDetectInput(blocking,-1);
            }

            //__________________________________________________________________
            //
            // broadcast visitstate
            //__________________________________________________________________
            MPI.Bcast(visitstate,0,MPI_COMM_WORLD);
            MPI.Printf(stderr,"visitstate=%d\n",visitstate);

            //__________________________________________________________________
            //
            //
            // process input
            //
            //__________________________________________________________________
            switch(visitstate)
            {

                case 0:
                    //__________________________________________________________
                    //
                    // will step
                    //__________________________________________________________
                    MPI.Printf0(stderr, "STEP\n");
                    break;
                    
                case 1:
                    //__________________________________________________________
                    //
                    // attempting connection
                    //__________________________________________________________
                    if(VisItAttemptToCompleteConnection())
                    {
                        MPI.Printf0(stderr,"%sconnected\n",fn);
                        VisItSetSlaveProcessCallback(SlaveProcessCallback);
                    }
                    else
                    {
                        MPI.Printf0(stderr,"%sdit not connect\n",fn);
                    }
                    break;

                case 2:
                    //__________________________________________________________
                    //
                    // Trying to tell something
                    //__________________________________________________________
                    runMode = VISIT_SIMMODE_STOPPED;
                    if(!ProcessVisItCommand(*this))
                    {
                        VisItDisconnect();
                        runMode = VISIT_SIMMODE_RUNNING;
                    }
                    break;


                default:
                    throw exception("VisIt failure code=%d",visitstate);
            }


        }

    }

}
