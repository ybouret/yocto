#include "yocto/visit/interface.hpp"

namespace yocto
{

    void VisIt:: Simulation:: loop()
    {

        while(!done)
        {
            const int blocking   = (VISIT_SIMMODE_STOPPED == runMode) ? 1 : 0;
            int       visitstate = 0;

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
                    


                default:
                    throw exception("VisIt failure code=%d",visitstate);
            }


        }

    }

}
