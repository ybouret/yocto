#include "yocto/visit/interface.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/string/tokenizer.hpp"

namespace yocto
{

    static const char *fn = VisIt::name;

#define VISIT_COMMAND_FAILURE (-1)
#define VISIT_COMMAND_SUCCESS (0)
#define VISIT_COMMAND_PROCESS (1)

    static inline
    void BroadcastSlaveCommand(int *command)
    {
        assert(command);
        if( MPI_SUCCESS != MPI_Bcast(command, 1, MPI_INT, 0, MPI_COMM_WORLD) )
        {
            throw exception("[%s] BroadcastSlaveCommand",fn);
        }
    }

    static inline
    void SlaveProcessCallback()
    {
        int command = VISIT_COMMAND_PROCESS;
        BroadcastSlaveCommand(&command);
    }

    //__________________________________________________________________________
    //
    // dispatch Viewer's command to CPU
    //__________________________________________________________________________
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
                        throw exception("[%s] unexpected command=%d",fn,command);
                }
            }
        }
        // never get here...
        return true;
    }

    //__________________________________________________________________________
    //
    //
    // Preparing Meta Data I/O
    //
    //__________________________________________________________________________
    static inline
    visit_handle SimGetMetaData(void *addr)
    {
        assert(addr);
        visit_handle       md  = VISIT_INVALID_HANDLE;
        VisIt::Simulation &sim = *static_cast<VisIt::Simulation *>(addr);

        //______________________________________________________________________
        //
        // prepare the handle
        //______________________________________________________________________
        if( VisIt_SimulationMetaData_alloc(&md) != VISIT_OKAY)
        {
            return VISIT_INVALID_HANDLE;
        }

        //______________________________________________________________________
        //
        // common status
        //______________________________________________________________________

        {
            VisIt_SimulationMetaData_setMode(md,sim.runMode);
            VisIt_SimulationMetaData_setCycleTime(md,sim.cycle,sim.runTime);
        }

        //______________________________________________________________________
        //
        // default generic command
        //______________________________________________________________________
        sim.addGenericCommands(md);

        //______________________________________________________________________
        //
        // Simulation MetaData
        //______________________________________________________________________
        sim.setMetaData(md);

        return md;
    }

    //__________________________________________________________________________
    //
    //
    // Preparing Mesh I/O
    //
    //__________________________________________________________________________
    static inline
    visit_handle SimGetMesh(int domain,const char *name, void *addr)
    {
        assert(addr);
        VisIt::Simulation &sim   = *static_cast<VisIt::Simulation *>(addr);
        const string       mesh_name = name;
        return sim.getMesh(domain,mesh_name);
    }

    //__________________________________________________________________________
    //
    //
    // Preparing Variables I/O
    //
    //__________________________________________________________________________
    static inline
    visit_handle SimGetVariable(int domain, const char *name, void *addr )
    {
        assert(addr);
        const string variable_name = name;
        return static_cast<VisIt::Simulation *>(addr)->getVariable(domain,variable_name);
    }

    //__________________________________________________________________________
    //
    //
    // Preparing Curves I/Io
    //
    //__________________________________________________________________________
    static inline
    visit_handle SimGetCurve(const char *name,void *addr)
    {
        assert(addr);
        const string id = name;
        return static_cast<VisIt::Simulation *>(addr)->getCurve(id);
    }


    //__________________________________________________________________________
    //
    //
    // Console/Control
    //
    //__________________________________________________________________________
    static inline bool isSeparator(char C) throw()
    {
        switch(C)
        {
            case ' ':
            case ';':
            case '\r':
            case '\n':
                return true;

            default:
                break;
        }
        return false;
    }

    void VisIt::Simulation:: on_quit(YOCTO_VISIT_SIMULATION_CALLBACK_PROTO) throw()
    {
        done = true;
    }

    void VisIt::Simulation:: on_halt(YOCTO_VISIT_SIMULATION_CALLBACK_PROTO) throw()
    {
        runMode = VISIT_SIMMODE_STOPPED;
    }

    void VisIt::Simulation:: on_run(YOCTO_VISIT_SIMULATION_CALLBACK_PROTO) throw()
    {
        runMode = VISIT_SIMMODE_RUNNING;
    }

    void VisIt::Simulation:: on_step(YOCTO_VISIT_SIMULATION_CALLBACK_PROTO)
    {
        // TODO: check args
        if(args.size()>0)
        {
            step();
        }
        else
        {
            step();
        }
        runMode = VISIT_SIMMODE_STOPPED;
    }

    static inline
    void ProcessAnyCommand(const string &com,
                           void         *addr)
    {
        assert(addr);
        VisIt::Simulation &sim = *static_cast<VisIt::Simulation *>(addr);
        const mpi         &MPI = sim.MPI;
        //______________________________________________________________________
        //
        // parse command
        //______________________________________________________________________
        MPI.Printf(stderr,"COMMAND=[%s]\n",com.c_str());


        vector<string> args(16,as_capacity);
        const string   cmd = tokenizer::get_command(args,com,isSeparator);
        if(MPI.IsFirst)
        {
            fflush(stderr);
            for(size_t i=1;i<=args.size();++i)
            {
                MPI.Printf0(stderr, "arg#%u: '%s'\n", unsigned(i), args[i].c_str());
            }
        }

        VisIt::Simulation::CallbackInfo *hook = sim.callbacks.search(cmd);
        if(hook)
        {
            MPI.Printf0(stderr, "calling <%s>\n", cmd.c_str());
            hook->cb(cmd,args);
            sim.update();
        }
        else
        {
            MPI.Printf0(stderr, "no call to <%s>\n", cmd.c_str());
        }
    }

    //__________________________________________________________________________
    //
    //
    // ControlCommand
    //
    //__________________________________________________________________________
    static inline
    void ControlCommandCallback(const char *cmd_,
                                const char *args_,
                                void       *addr)
    {
        assert(addr);
        string cmd = cmd_;
        cmd << ';' << args_;
        
        ProcessAnyCommand(cmd,addr);
    }


    static inline bool isENDL(char C) throw()
    {
        return ('\n' == C) || ('\r' == C);
    }

    //__________________________________________________________________________
    //
    //
    // dispatch console
    //
    //__________________________________________________________________________
    static inline
    void ProcessConsoleInput(void *addr)
    {
        assert(addr);
        VisIt::Simulation &sim = *static_cast<VisIt::Simulation *>(addr);
        const mpi         &MPI = sim.MPI;


        string cmd;
        if(MPI.IsFirst)
        {
            char buffer[1024];
            memset(buffer,0,sizeof(buffer));
            const bool   isEOF = (NULL == fgets(buffer,sizeof(buffer)-1,stdin));
            if(isEOF)
            {
                memcpy(buffer,"quit",5);
            }
            cmd = buffer;
            cmd.trim(isENDL);
        }
        MPI.Bcast(cmd, 0, MPI_COMM_WORLD);

        MPI.Printf(stderr, "console=[%s]\n", cmd.c_str());
        ProcessAnyCommand(cmd,addr);
    }


    //__________________________________________________________________________
    //
    //
    // DomainList
    //
    //__________________________________________________________________________

    static inline
    visit_handle SimGetDomainList(const char *id,void *addr)
    {
        assert(addr);
        VisIt::Simulation &sim = *static_cast<VisIt::Simulation *>(addr);
        const mpi         &MPI = sim.MPI;
        if(id)
        {
            MPI.Printf(stderr, "SimGetDomainList id='%s'\n", id);
        }
        else
        {
            MPI.Printf(stderr,"SimGetDomainList id=NULL\n");
        }
        visit_handle h = VISIT_INVALID_HANDLE;
        if(VISIT_ERROR != VisIt_DomainList_alloc(&h) )
        {
            visit_handle hdl = VISIT_INVALID_HANDLE;
            if(VISIT_ERROR != VisIt_VariableData_alloc(&hdl) )
            {
                VisIt_VariableData_setDataI(hdl, VISIT_OWNER_SIM, 1, 1, (int *) &MPI.CommWorldRank);
                VisIt_DomainList_setDomains(h,MPI.CommWorldSize,hdl);
            }

        }
        return h;
    }

#if 0
    //__________________________________________________________________________
    //
    //
    // DomainBoundaries
    //
    //__________________________________________________________________________
    static inline
    visit_handle SimGetDomainBoundaries(const char *mesh_name,
                                        void       *addr)
    {
        return VISIT_INVALID_HANDLE;
    }
#endif
    

    //__________________________________________________________________________
    //
    //
    // Main Loop
    //
    //__________________________________________________________________________

    void VisIt:: Simulation:: loop()
    {

        while(!done)
        {
            const int blocking   = (VISIT_SIMMODE_STOPPED == runMode) ? 1 : 0;
            int       visitstate = 0;
            (int &)   connected    = VisItIsConnected();
            //MPI.Printf0(stderr, "[%s] blocking=%d, connected=%d\n",fn,blocking,connected);
            if(blocking&&MPI.IsFirst)
            {
                MPI.Printf0(stderr, "Ready[%c]> ", (connected!=0?'+' : '-'));
                fflush(stderr);
            }


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
                visitstate = VisItDetectInput(blocking,fileno(stdin));
            }

            //__________________________________________________________________
            //
            // broadcast visitstate
            //__________________________________________________________________
            MPI.Bcast(visitstate,0,MPI_COMM_WORLD);

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
                    // will step, doesn't change status
                    //__________________________________________________________
                    step();
                    break;
                    
                case 1:
                    //__________________________________________________________
                    //
                    // attempting connection and install callbacks
                    //__________________________________________________________
                    if(VisItAttemptToCompleteConnection())
                    {
                        MPI.Printf0(stderr,"[%s] CONNECTED\n",fn);
                        VisItSetSlaveProcessCallback(SlaveProcessCallback);
                        VisItSetGetMetaData(SimGetMetaData,this);
                        VisItSetCommandCallback(ControlCommandCallback,this);
                        VisItSetGetDomainList(SimGetDomainList,this);
                        VisItSetGetMesh(SimGetMesh,this);
                        VisItSetGetVariable(SimGetVariable,this);
                        //VisItSetGetDomainBoundaries(SimGetDomainBoundaries,this);
                        VisItSetGetCurve(SimGetCurve,this);
                    }
                    else
                    {
                        if(MPI.IsFirst)
                        {
                            const string errMsg = VisItGetLastError();
                            MPI.Printf0(stderr,"[%s] DIT NOT CONNECT: %s\n",fn,errMsg.c_str());
                        }
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

                case 3:
                    //__________________________________________________________
                    //
                    // Console Input
                    //__________________________________________________________
                    ProcessConsoleInput(this);
                    break;

                default:
                    throw exception("[%s] failure code=%d",fn,visitstate);
            }


        }

    }

}
