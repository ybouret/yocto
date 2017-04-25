#include "yocto/visit/interface.hpp"

namespace yocto
{


    static inline int visit_broadcast_int_callback(int *value, int sender) throw()
    {
        return MPI_Bcast(value, 1, MPI_INT, sender, MPI_COMM_WORLD);
    }
    static inline int visit_broadcast_string_callback(char *str,
                                                      int   len,
                                                      int   sender) throw()
    {
        return MPI_Bcast(str, len, MPI_CHAR, sender, MPI_COMM_WORLD);
    }

    const char VisIt::name[] = "VisIt";

    namespace
    {
        static const char * __sim_name  = 0;
        static const char * __sim_comm  = 0;
        static const mpi *  __MPI       = 0;
        static bool         __sim_trace = false;
        static const char * __sim_gui   = 0;
    }

    VisIt :: ~VisIt() throw()
    {
        if(MPI.IsFirst)
        {
            VisItCloseTraceFile();
        }
    }

    VisIt & VisIt:: Start(const string &sim_name,
                          const string &sim_comm,
                          const mpi    &usrMPI,
                          const string *sim_gui,
                          const bool    usrTrace)
    {
        YOCTO_LOCK(access);
        __sim_name  = sim_name.c_str();
        __sim_comm  = sim_comm.c_str();
        __MPI       = &usrMPI;
        __sim_trace = usrTrace;
        __sim_gui   = (sim_gui != NULL) ? sim_gui->c_str() : NULL;
        return VisIt::instance();
    }

    VisIt:: VisIt() :
    MPI(*__MPI)
    {
        //______________________________________________________________________
        //
        // preparing visit
        //______________________________________________________________________
        MPI.Printf(stderr, "%s Setup on %d.%d\n",name,MPI.CommWorldSize,MPI.CommWorldRank);

        if(__sim_trace && MPI.IsFirst)
        {
            string trace_file = "trace-";
            trace_file += __sim_name;
            trace_file += ".txt";
            VisItOpenTraceFile(trace_file.c_str());
        }

        if(!VisItSetupEnvironment())
        {
            throw exception("%s: in VisItSetupEnvironment",name);
        }
        VisItSetBroadcastIntFunction(visit_broadcast_int_callback);
        VisItSetBroadcastStringFunction(visit_broadcast_string_callback);
        VisItSetParallel(MPI.IsParallel?1:0);
        VisItSetParallelRank(MPI.CommWorldRank);

        if(MPI.IsFirst)
        {
            if(!VisItInitializeSocketAndDumpSimFile(__sim_name,
                                                    __sim_comm,
                                                    NULL, // path
                                                    NULL, // inputfile
                                                    NULL, // guifile
                                                    NULL  // absolute path
                                                    )
               )
            {
                throw exception("%s: in VisItInitializeSocketAndDumpSimFile",name);
            }
        }

        MPI.Printf(stderr, "%s Ready on %d.%d\n",name,MPI.CommWorldSize,MPI.CommWorldRank);        
    }
    
}
