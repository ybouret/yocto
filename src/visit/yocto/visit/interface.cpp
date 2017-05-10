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
        MPI.Printf(stderr, "%s Setup on %d.%d...\n",name,MPI.CommWorldSize,MPI.CommWorldRank);

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
                                                    NULL,      // path
                                                    NULL,      // inputfile
                                                    __sim_gui, // guifile
                                                    NULL       // absolute path
                                                    )
               )
            {
                throw exception("%s: in VisItInitializeSocketAndDumpSimFile",name);
            }
        }

        MPI.Printf(stderr, "%s Ready on %d.%d !\n",name,MPI.CommWorldSize,MPI.CommWorldRank);
    }

}

namespace yocto
{

    visit_handle VisIt:: VariableData_alloc()
    {
        visit_handle h = VISIT_INVALID_HANDLE;
        if( VISIT_OKAY != VisIt_VariableData_alloc(&h) )
        {
            throw exception("VisIt_VariableData_alloc");
        }
        return h;
    }

    void VisIt:: VariableData_free( visit_handle &h ) throw()
    {
        (void) VisIt_VariableData_free(h);
        h = VISIT_INVALID_HANDLE;
    }

    template <>
    void VisIt:: VariableData_Set<float>(visit_handle &h,
                                         const float *arr,
                                         const size_t nTuples)
    {
        if(VISIT_OKAY!=VisIt_VariableData_setDataF(h,VISIT_OWNER_SIM,1, nTuples, (float *)arr))
        {
            throw exception("VisIt_VariableData_setDataF");
        }
    }

    template <>
    void VisIt:: VariableData_Set<double>(visit_handle &h,
                                          const double *arr,
                                          const size_t  nTuples)
    {
        if(VISIT_OKAY!=VisIt_VariableData_setDataD(h,VISIT_OWNER_SIM,1, nTuples, (double *)arr))
        {
            throw exception("VisIt_VariableData_setDataD");
        }
    }

    template <>
    void VisIt:: VariableData_Set< point2d<float> >(visit_handle         &h,
                                                    const point2d<float> *arr,
                                                    const size_t          nTuples)
    {
        if(VISIT_OKAY!=VisIt_VariableData_setDataF(h,VISIT_OWNER_SIM,2, nTuples, (float *)arr))
        {
            throw exception("VisIt_VariableData_setDataF/point2d");
        }
    }

    template <>
    void VisIt:: VariableData_Set< point2d<double> >(visit_handle         &h,
                                                    const point2d<double> *arr,
                                                    const size_t          nTuples)
    {
        if(VISIT_OKAY!=VisIt_VariableData_setDataD(h,VISIT_OWNER_SIM,2, nTuples, (double *)arr))
        {
            throw exception("VisIt_VariableData_setDataD/point2d");
        }
    }


    
}

namespace yocto
{

    visit_handle VisIt:: MeshMetaData_alloc()
    {
        visit_handle h = VISIT_INVALID_HANDLE;
        if( VISIT_OKAY != VisIt_MeshMetaData_alloc(&h) )
        {
            throw exception("VisIt_MeshMetaData_alloc");
        }
        return h;
    }

    void VisIt:: MeshMetaData_free(visit_handle &h) throw()
    {
        (void) VisIt_MeshMetaData_free(h);
        h = VISIT_INVALID_HANDLE;
    }

}


