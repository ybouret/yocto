#include "yocto/visit/sim.hpp"

namespace yocto
{

    void VisIt:: loop()
    {

        do
        {
            const int blocking   = (VISIT_SIMMODE_STOPPED == runMode) ? 1 : 0;
            int       visitstate = 0;

            //__________________________________________________________________
            //
            //
            // get input from VisIt or timeout
            //
            //__________________________________________________________________

            if(par_main)
            {
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
        while(!done);
        
    }

}
