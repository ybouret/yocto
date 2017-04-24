#include "yocto/visit/interface.hpp"

namespace yocto
{
    VisIt :: ~VisIt() throw()
    {
    }

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
        static const char * __sim_name = 0;
        static const char * __sim_comm = 0;
        static const mpi * __MPI       = 0;
    }

    VisIt & VisIt:: Start(const string &sim_name,
                          const string &sim_comm,
                          const mpi    &usrMPI)
    {
        YOCTO_LOCK(access);
        __sim_name = sim_name.c_str();
        __sim_comm = sim_comm.c_str();
        __MPI      = &usrMPI;
        return VisIt::instance();
    }

    VisIt:: VisIt() :
    MPI(*__MPI)
    {
        //______________________________________________________________________
        //
        // preparing visit
        //______________________________________________________________________
        MPI.Printf(stderr, "Visit Setup on %d.%d\n",MPI.CommWorldSize,MPI.CommWorldRank);
        VisItSetupEnvironment();
        VisItSetBroadcastIntFunction(visit_broadcast_int_callback);
        VisItSetBroadcastStringFunction(visit_broadcast_string_callback);
        VisItSetParallel(MPI.IsParallel?1:0);
        VisItSetParallelRank(MPI.CommWorldRank);

        if(MPI.IsFirst)
        {
            VisItInitializeSocketAndDumpSimFile(__sim_name,
                                                __sim_comm,
                                                NULL, // path
                                                NULL, // inputfile
                                                NULL, // guifile
                                                NULL  // absolute path
                                                );
        }

        
    }

}
