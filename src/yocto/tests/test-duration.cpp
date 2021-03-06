#include "yocto/utest/run.hpp"
#include "yocto/duration.hpp"
#include "yocto/code/alea.hpp"

using namespace yocto;

static void show_duration( const double seconds )
{
	const duration d(seconds);
	std::cerr << seconds << " => " << d << std::endl;
}

#define SHOW(s) show_duration(s)

YOCTO_UNIT_TEST_IMPL(duration)
{
	SHOW(10.1);
	SHOW(59);
	SHOW(60);
	SHOW(1000);
	SHOW(10000);
	SHOW(100000);
}
YOCTO_UNIT_TEST_DONE()

#include "yocto/eta.hpp"

YOCTO_UNIT_TEST_IMPL(eta)
{
	eta ETA;
	double X = 0;
	ETA.reset();
	
	while( X < 1 )
	{
		wtime::sleep( 0.02 * alea.get<double>() );
		X += 0.01 * alea.get<double>();
		ETA(X);
        ETA.progress(X);
	}
    ETA.progress_flush();
}
YOCTO_UNIT_TEST_DONE()

