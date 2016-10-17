#include "sim.hpp"

namespace yocto
{
    VisIt :: ~VisIt() throw()
    {
    }

    static int visit_broadcast_int_callback(int *value, int sender) throw()
    {
        return MPI_Bcast(value, 1, MPI_INT, sender, MPI_COMM_WORLD);
    }
    static int visit_broadcast_string_callback(char *str,
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
                          const mpi &MPI)
    {
        YOCTO_LOCK(access);
        __sim_name = sim_name.c_str();
        __sim_comm = sim_comm.c_str();
        __MPI      = &MPI;
        return VisIt::instance();
    }

    VisIt:: VisIt() :
    cycle(0),
    time(0),
    runMode(0),
    done(0),
    par_rank(__MPI->CommWorldRank),
    par_size(__MPI->CommWorldSize),
    parallel(par_size>1),
    par_main(0==par_rank)
    {
        //______________________________________________________________________
        //
        // preparing visit
        //______________________________________________________________________
        VisItSetupEnvironment();
        VisItSetBroadcastIntFunction(visit_broadcast_int_callback);
        VisItSetBroadcastStringFunction(visit_broadcast_string_callback);
        VisItSetParallel(parallel);
        VisItSetParallelRank(par_rank);

        if(par_main)
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
