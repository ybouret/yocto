#include "yocto/eta.hpp"
#include "yocto/duration.hpp"
#include "yocto/code/utils.hpp"

#include <iostream>
#include <cstdio>

namespace yocto
{
	
	static const double max_seconds = duration::max_days * ( 24 * 60 * 60 );
	
	eta:: ~eta() throw() {}
	
	
	eta:: eta() : 
	time_done(0),
	time_left(max_seconds),
	ratio_done(0),
	ratio_left(1),
	chrono_()
	{
		chrono_.start();
	}
	
	void eta:: reset()
	{
		(double &)time_done  = 0;
		(double &)time_left  = max_seconds;
		(double &)ratio_done = 0;
		(double &)ratio_left = 1;
		chrono_.start();
	}
	
	double eta:: now()
	{
		return chrono_.query();
	}
	
	void   eta:: operator()( double ratio )
	{
		(double &)ratio_done = clamp<double>(0,ratio,1);
		(double &)ratio_left = 1.0 - ratio_done;
		(double &)time_done  = now();
		const double num = ratio_left * time_done;
		if( num >= ratio_done * max_seconds )
			(double &)time_left = max_seconds;
		else 
			(double &)time_left = num / ratio;
	}

    void eta:: progress(double ratio)
    {
        static const char wheels[] = { '-','\\','|','/' };
        static const char nwheel   = sizeof(wheels)/sizeof(wheels[0]);
        static unsigned   iwheel   = 0;
        eta &self = *this;
        self(ratio);
        duration done(time_done);
        duration left(time_left);
        std::cerr.flush();
        const char C = (ratio<1.0) ? wheels[(iwheel++)%nwheel] : '#';
        fprintf(stderr, "[%c] %6.2f%% in ",C, 100.0*ratio_done);
        if(done.d>0)
        {
            fprintf(stderr,"%02dD+",done.d);
        }
        fprintf(stderr,"%02dh%02dm%05.2fs",done.h,done.m,done.s);
        fprintf(stderr, " | ETA=" );
        if(left.d>0)
        {
            fprintf(stderr,"%02dD+",left.d);
        }
        fprintf(stderr,"%02dh%02dm%05.2fs",left.h,left.m,left.s);
        fprintf(stderr,"          \r");
        fflush(stderr);
    }

    void eta::progress_flush()
    {
        std::cerr.flush();
        fprintf(stderr,"\n");
        fflush(stderr);
    }
}
