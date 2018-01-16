#include "yocto/utest/run.hpp"

#include "yocto/mpi/mpi.hpp"
#include "yocto/associative/type-glossary.hpp"
#include "yocto/ios/null-device.hpp"

using namespace yocto;

YOCTO_UNIT_TEST_IMPL(init)
{
    singleton_verbosity = true;
    (void)ios::null_device::instance();

    YOCTO_MPI_ENV();
    //MPI.CloseStdIO();
    MPI.Printf(stderr, "'%s' is ready\n",MPI.ProcessorName);
	MPI.Printf0(stderr, "ThreadLevel= %s\n", MPI.ThreadLevelName());

    type_glossary<MPI_Datatype> tg;
    tg.enroll<char>(MPI_CHAR);
    tg.enroll<int8_t>(MPI_INT8_T);
    

}

YOCTO_UNIT_TEST_DONE()
